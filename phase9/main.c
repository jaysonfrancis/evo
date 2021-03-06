// main.c, 159
//
// Team Name: Evo (Members: Aaron Blancaflor and Jayson Francis)

#include "spede.h"      // spede stuff
#include "main.h"       // main stuff
#include "isr.h"        // ISR's
#include "tool.h"       // handy functions
#include "proc.h"       // processes such as Init()
#include "type.h"       // processes such as Init()
#include "entry.h"
#include "TF.h"

//added printsemaphore and print it for phase 4
int CRP, sys_time, print_it,print_semaphore;// current running PID, -1 means no process
// product_semaphore,product only used for testing will delete later
q_t run_q, none_q,sleep_q,semaphore_q; // processes ready to run and not used
semaphore_t semaphore[Q_SIZE]; //added in phase 3
pcb_t pcb[MAX_PROC];    // process table
char stack[MAX_PROC][STACK_SIZE]; // run-time stacks for processes
struct i386_gate *IDT_ptr;
terminal_t terminal;
mbox_t mbox[MAX_PROC]; // Mailbox ID is the Process ID
page_t page[MAX_PROC];

int sys_main_table;

void SetEntry(int entry_num, func_ptr_t func_ptr){
   struct i386_gate *gateptr = &IDT_ptr[entry_num];
   fill_gate(gateptr,(int)func_ptr,get_cs(),ACC_INTR_GATE,0);
}

void InitIDT(){
   IDT_ptr = get_idt_base();//locate IDT
   cons_printf("IDT is at %u. \n",IDT_ptr);
   SetEntry(32,TimerEntry);
   SetEntry(48,GetPidEntry);
   SetEntry(49,SleepEntry);
   SetEntry(50,SemWaitEntry);
   SetEntry(51,SemPostEntry);
   SetEntry(52,SemGetEntry); //program into entry.S
   SetEntry(MSGSND_INTR,MsgSndEntry);
   SetEntry(MSGRCV_INTR,MsgRcvEntry);
   SetEntry(IRQ7_INTR, IRQ7Entry); //program into entry.S
   SetEntry(IRQ3_INTR, IRQ3Entry);
   SetEntry(FORK_INTR, ForkEntry);
   SetEntry(WAIT_INTR, WaitEntry);
   SetEntry(EXIT_INTR, ExitEntry);
   outportb(0x21,~(128+8+1));
}



void InitData() {
   int i,j;
   sys_time = 0;
   sys_main_table = get_cr3(); // Tip 1 in Phase 9
   //initializing 4 queues
   MyBZero((char *) &run_q,sizeof(run_q));
   MyBZero((char *) &none_q,sizeof(none_q));
   MyBZero((char *) &sleep_q,sizeof(sleep_q));
   MyBZero((char *) &semaphore_q,sizeof(semaphore_q));
   
   for(i = 1 ; i<Q_SIZE;i++){
      pcb[i].state = NONE;
      EnQ(i,&none_q);
      EnQ(i,&semaphore_q); // added in phase3
   }
   CRP = 0;
   print_it = 0;//4

   for(j = 1; j<MAX_PROC;j++){
      page[j].owner = -1;
      page[j].addr = 0xE00000 + 0x1000 * j;
   }

}

void SelectCRP() {       // select which PID to be new CRP
   if(CRP > 0){
      return;
   }    
   //if it's' 0 (Idle), change its state in PCB to RUN
   if(CRP == 0){
      pcb[0].state = RUN;
   }

   if(run_q.size == 0 ){
      CRP = 0;
   }else{  
      CRP = DeQ(&run_q); 
   }
      pcb[CRP].mode = UMODE;
      pcb[CRP].state = RUNNING;
   
}
int main() {
   int i,j,k;
   InitData();       //call Init Data to initialize kernel data
   CreateISR(0);  //call CreateISR(0) to create Idle process (PID 0)
   i=DeQ(&none_q);
   CreateISR(i);
   j=DeQ(&none_q);
   CreateISR(j);
   k = DeQ(&none_q);
   CreateISR(k);
   i=DeQ(&none_q);
   CreateISR(i);
   j=DeQ(&none_q);
   CreateISR(j);
   k = DeQ(&none_q);
   CreateISR(k);
   InitIDT();
   Dispatch(pcb[0].TF_ptr);    // to dispatch/run CRP
   
   return 0;
}
void Kernel(TF_t *TF_ptr) {


   
   pcb[CRP].TF_ptr=TF_ptr;
   //change state in PCB of CRP to kernel mode
   pcb[CRP].mode = KMODE;
   //call TimerISR() to service timer interrupt as it just occurred
   switch(TF_ptr->intr_num){ 

      case TIMER_INTR:
         TimerISR();
         break;
      case GETPID_INTR:
         GetPidISR();
         break;
      case SLEEP_INTR:
         SleepISR(TF_ptr->ebx);
         break;
      case SEMPOST_INTR:
         SemPostISR(TF_ptr->ebx);
         break;
      case SEMWAIT_INTR:
         SemWaitISR(TF_ptr->ebx);
         break;
      case SEMGET_INTR: //4
         pcb[CRP].TF_ptr->ecx = SemGetISR(pcb[CRP].TF_ptr->ebx);

         break;
      case MSGRCV_INTR:
         MsgRcvISR(); //pass something into it?
         break;
      case MSGSND_INTR:
         MsgSndISR(); //pass something into it?
         break;
      case IRQ3_INTR:
         IRQ3ISR();
         break;   
      case IRQ7_INTR: //4
         IRQ7ISR();
         break;   
      case FORK_INTR:
         ForkISR();
      break;
      case WAIT_INTR:
         WaitISR();
      break;      
      case EXIT_INTR:
         ExitISR();
      break; 
      default:
         cons_printf("Something went wrong\n");
         breakpoint();
         break;
   }
 
   SelectCRP();    //call SelectCRP() to settle/determine for next CRP
   set_cr3(pcb[CRP].main_table); // Phase 9, tip #4 - set the MMU to start using the designated main table for runtime memory address translation
   Dispatch(pcb[CRP].TF_ptr);
}

