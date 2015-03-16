/**
 * @copyright (C) 2015 by Team LeftShark CSUS CPE159
 * @file   isr.c
 * @author Brandon Ortiz (brandon@velwave.com)
 * @author Benjamin Smith (benjamin.smith.cs@gmail.com)
 * @date   February 16, 2015
 * @brief  
 *
 */

/****************************************************************************
 *
 * Place #include files required to compile this source file here.
 *
 ****************************************************************************/

#include "spede.h"
#include "type.h"
#include "isr.h"
#include "tool.h"
#include "extern.h"
#include "proc.h"
#include "syscall.h"

/****************************************************************************
 *
 * Define global variables that will be used by both this source file
 * and by other source files here.
 *
 ****************************************************************************/

/****************************************************************************
 *
 * Place #define constants and macros that will be used only by this
 * source file here.
 *
 ****************************************************************************/
#ifdef VM
   #define SLEEP_SCALAR 1
#else
   #define SLEEP_SCALAR 100
#endif
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

/** @brief Create an ISR
 *
 *  @param pid 
 *  @return void
 *
 */
void CreateISR(const int pid) 
{
      pcb[pid].state = RUN;
      pcb[pid].mode = UMODE;
      pcb[pid].runtime = 0;
      pcb[pid].total_runtime = 0;

      EnQ(pid, &run_q);

      MyBZero(stack[pid], STACK_SIZE); // erase stack

      // point to just above stack, then drop by sizeof(TF_t)
      pcb[pid].TF_ptr = (TF_t *)&stack[pid][STACK_SIZE];
      pcb[pid].TF_ptr--;

      // fill out trapframe of this new proc:
      if (pid == 0)
      {
         pcb[pid].TF_ptr->eip = (unsigned int)Idle; // Idle process
      }
      else if (pid == 1)
      {
         pcb[pid].TF_ptr->eip = (unsigned int) PrintDriver;
      }
      else
      {
         pcb[pid].TF_ptr->eip = (unsigned int) UserProc;
      }

      pcb[pid].TF_ptr->eflags = EF_DEFAULT_VALUE | EF_INTR;
      pcb[pid].TF_ptr->cs = get_cs();
      pcb[pid].TF_ptr->ds = get_ds();
      pcb[pid].TF_ptr->es = get_es();
      pcb[pid].TF_ptr->fs = get_fs();
      pcb[pid].TF_ptr->gs = get_gs();   
}

/** @brief Terminate an ISR
 *
 *  @return void
 *
 */
void TerminateISR(void) 
{
   if ((CRP > 0) && (CRP < MAX_PROC))
   {
      pcb[CRP].state = NONE;

      EnQ(CRP, &none_q);

      CRP = -1;
   }
}  

/** @brief Timer interrupt handler
 *
 *  @return void
 *
 */
void TimerISR(void) 
{
   int pid_sleeping;
   int sleeping_pids[MAX_PROC];
   unsigned int number_sleeping = 0;   
   unsigned int i;

   outportb(0x20, 0x60); //clear timer interrupt so next one will be recognized

   pcb[CRP].runtime++;
   sys_time++;

   //Scan sleep queue and move pids ready to wake into run_q
   pid_sleeping = DeQ(&sleep_q);
   asrt(pid_sleeping <= MAX_PROC, "Invalid number retrieved from sleep q in timerISR %d", pid_sleeping)

   while (pid_sleeping >= 0)
   {
      sleeping_pids[number_sleeping++] = pid_sleeping;

      pid_sleeping = DeQ(&sleep_q);
   }

   if (number_sleeping)
   {
      for (i = 0; i < number_sleeping; i++)
      {
         pid_sleeping = sleeping_pids[i];         

         if (pid_sleeping < MAX_PROC)
         {
            if (sys_time >= pcb[pid_sleeping].wake_time)
            {
               pcb[pid_sleeping].state = RUN;

               EnQ(pid_sleeping, &run_q);            
            }
            else
            {
               EnQ(pid_sleeping, &sleep_q);
            }
         }
      }
   }

   if (pcb[CRP].runtime >= TIME_LIMIT)
   {  
      pcb[CRP].total_runtime += pcb[CRP].runtime;

      pcb[CRP].runtime = 0;

      pcb[CRP].state = RUN;

      EnQ(CRP, &run_q);

      CRP = -1;
   }
}

/** @brief returns PID of currently running process
 *
 *  @return int CRP
 *
 */
void GetPidISR(void) 
{
   pcb[CRP].TF_ptr->ebx = CRP;
}

/** @brief 
 *
 *  @return void
 *  @param s number of seconds to sleep
 */
void SleepISR(int s) 
{
   pcb[CRP].wake_time = sys_time + (s * SLEEP_SCALAR);

   pcb[CRP].state = SLEEP;

   EnQ(CRP, &sleep_q);

   CRP = -1;
}

/** @brief 
 *
 *  @return
 *
 */
void SemWaitISR(int semaphoreID) 
{
   if (semaphoreID < Q_SIZE)
   {
      if (semaphore[semaphoreID].count > 0)
      {
         semaphore[semaphoreID].count--;
      }
      else
      {
         EnQ(CRP, &(semaphore[semaphoreID].wait_q));

         pcb[CRP].state = WAIT;

         CRP = -1;
      }
   }
}

/** @brief 
 *
 *  @return
 *
 */
void SemPostISR(int semaphoreID) 
{
   int pid_freed;

   if (semaphoreID < Q_SIZE)
   {
      pid_freed = DeQ(&(semaphore[semaphoreID].wait_q));

      if (pid_freed == -1)
      {
         semaphore[semaphoreID].count++;
      }
      else
      {
         pcb[pid_freed].state = RUN;
         EnQ(pid_freed, &run_q);    
      }
   }
}

/** @brief 
 *
 *  @return
 *
 */
int SemGetISR(int count) 
{
   int semaphore_id;

   semaphore_id = DeQ(&semaphore_q);

   if (semaphore_id >= 0)
   {
      MyBZero((char *) &semaphore[semaphore_id], sizeof(semaphore_t));

      semaphore[semaphore_id].count = count;
   }

   return semaphore_id;
}

/** @brief IRQ7 interrupt handler
 *
 *  @return void
 *
 */
void IRQ7ISR(void) 
{
   outportb(0x20, 0x67);

   if (print_semaphore >= 0)
   {
      SemPostISR(print_semaphore);
   }
}

/****************************************************************************
 *
 * End of file.
 *
 ****************************************************************************/
