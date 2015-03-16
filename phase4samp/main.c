/**
 * @copyright (C) 2015 by Team LeftShark CSUS CPE159
 * @file   main.c
 * @author Brandon Ortiz (brandon@velwave.com)
 * @author Benjamin Smith (benjamin.smith.cs@gmail.com)
 * @date   February 16, 2015
 * @brief  Simulated kernel
 *
 */

/****************************************************************************
 *
 * Place #include files required to compile this source file here.
 *
 ****************************************************************************/

#include "spede.h"      // spede stuff
#include "main.h"       // main stuff
#include "isr.h"        // ISR's
#include "type.h"       // processes such as Init()
#include "tool.h"       // handy functions
#include "proc.h"       // processes such as Init()
#include "entry.h"

/****************************************************************************
 *
 * Define global variables that will be used by both this source file
 * and by other source files here.
 *
 ****************************************************************************/

//! Current running PID, -1 means no process
int CRP;    

//! Run queue containing PIDs of processes running or ready to run
q_t run_q;  

//! None queue containing unused PIDs
q_t none_q; 

//! Process control block
pcb_t pcb[MAX_PROC];    

//! Run-time stacks for processes
char stack[MAX_PROC][STACK_SIZE];

//! pointer to Interrupt Description Table
struct i386_gate *IDT_ptr;

//! Phase 2 sys_time
volatile int sys_time;

//! Phase 2 sleep_q
q_t sleep_q;

//Phase 3
semaphore_t semaphore[Q_SIZE];
q_t semaphore_q;    

//Phase 4
int print_it;

/****************************************************************************
 *
 * Place #define constants and macros that will be used only by this
 * source file here.
 *
 ****************************************************************************/

/****************************************************************************
 *
 * Place typedefs, structs, unions, and enums that will be used only
 * by this source file here.
 *
 ****************************************************************************/

/****************************************************************************
 *
 * Place static variables that will be used only by this file here.
 *
 ****************************************************************************/

/****************************************************************************
 *
 * Place function prototypes that will be used only by this file here.
 * This section need only include "forward referenced" functions.
 *
 ****************************************************************************/

/****************************************************************************
 *
 * Place function bodies here.
 *
 ****************************************************************************/

/** @brief setup data for interrupt descriptor tabled
 *
 *  @return void
 *
 */
static void SetEntry(int entry_num, func_ptr_t func_ptr)
{
   struct i386_gate *gateptr =  &IDT_ptr[entry_num];

   fill_gate(gateptr, (int)func_ptr, get_cs(), ACC_INTR_GATE, 0);
}

/** @brief 
 *
 *  @return void
 *
 */
static void InitIDT(void)
{
   IDT_ptr = get_idt_base();         // function returns CPU register IDTR
   cons_printf("IDT is at %u.\n", IDT_ptr);

   SetEntry(TIMER_INTR, TimerEntry); // set interrupt vector for TimerEntry
   SetEntry(IRQ7_INTR, IRQ7Entry);
   outportb(0x21, ~0x81);               // set iterrupt mask to trigger on timer & IRQ7

   SetEntry(GETPID_INTR, GetPidEntry); // set interrupt vector for GetPidEntry
   SetEntry(SLEEP_INTR, SleepEntry); // set interrupt vector for SleepEntry

   SetEntry(SEMWAIT_INTR, SemWaitEntry);
   SetEntry(SEMPOST_INTR, SemPostEntry);
   SetEntry(SEMGET_INTR, SemGetEntry);

}

/** @brief Create an ISR
 *
 *  @return void
 *
 */
static void InitData(void) 
{
   int i;

   //Clear queues
   MyBZero((char *) &run_q, sizeof(run_q));
   MyBZero((char *) &none_q, sizeof(none_q));
   MyBZero((char *) &sleep_q, sizeof(sleep_q));
   MyBZero((char *) &semaphore_q, sizeof(semaphore_q));

   //Put all processes other than process 0 in the none queue and set state 
   //to NONE
   for (i = 1; i < MAX_PROC; i++)
   {
      EnQ(i, &none_q);
      EnQ(i, &semaphore_q);

      pcb[i].state = NONE;
   }

   //Set CRP to idle process
   CRP = 0;

   sys_time = 0;
}

/** @brief Create an ISR
 *
 *  @return void
 *
 */
int main(void) 
{
   InitData();
   InitIDT();
   CreateISR(0);

   CRP = 0;

   Dispatch(pcb[0].TF_ptr);

   while(1)
   {
      //Nothing to do
   };

   return 0;
}

/** @brief Select which PID to be new CRP
 *
 *  @return void
 *
 */
static void SelectCRP(void) 
{       
   int pid_current;
   int pid_next;

   pid_current = CRP;

   if (pid_current == 0)
   {
      pcb[pid_current].state = RUN;

      pid_current = -1;
   }

   if (pid_current == -1)
   {
      pid_next = DeQ(&run_q);

      if ((pid_next == -1) || (pid_next >= MAX_PROC))
      {
         CRP = 0;
      }
      else
      {
         CRP = pid_next;
      }

      pcb[CRP].mode = UMODE;
      pcb[CRP].state = RUNNING;
   }
}


/** @brief Main kernel
 *
 *  @return void
 *
 */
void Kernel(TF_t *TrapFrame) 
{
   int pid_new;
   char key;

   pcb[CRP].TF_ptr = TrapFrame;
   
   pcb[CRP].mode = KMODE;

   switch(pcb[CRP].TF_ptr->intr_num)
   {
      case TIMER_INTR:
         TimerISR();
      break;

      case IRQ7_INTR:
         IRQ7ISR();
      break;

      case GETPID_INTR:
         GetPidISR();
      break;

      case SLEEP_INTR:
         SleepISR(pcb[CRP].TF_ptr->eax);
      break;

      case SEMWAIT_INTR:
         SemWaitISR(pcb[CRP].TF_ptr->eax);
      break;

      case SEMPOST_INTR:
         SemPostISR(pcb[CRP].TF_ptr->eax);
      break;

      case SEMGET_INTR:
         pcb[CRP].TF_ptr->ebx = SemGetISR(pcb[CRP].TF_ptr->eax);
      break;

      default: 
         printf("non recognized ISR number");
         breakpoint();
      break;
   }

   //Process add/remove done by keyboard for simulation
   if(cons_kbhit())
   {
      key = cons_getchar();

      switch(key)
      {
         case 'n': //create new process
            pid_new = DeQ(&none_q); //remove from empty
            
            if (pid_new >= 0)
            {
               CreateISR(pid_new);
            }
            else
            {
               cons_printf("No processes left to process\n");
            }
         break;

         case 't':
            TerminateISR();
         break;

         case 'b':
            breakpoint();
         break;

         case 'p':   
            print_it = 1;            
         break;

         case 'r':   
            InitData();
         break;

         case 'q':
            exit(0);
         break;

         default:
            cons_printf("Unknown command\n");
         break;
      }
   }

   SelectCRP();

   Dispatch(pcb[CRP].TF_ptr);   
}

/****************************************************************************
 *
 * End of file.
 *
 ****************************************************************************/
