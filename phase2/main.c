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

int CRP, sys_time;                // current running PID, -1 means no process
q_t run_q, none_q,sleep_q;      // processes ready to run and not used
pcb_t pcb[MAX_PROC];    // process table
char stack[MAX_PROC][STACK_SIZE]; // run-time stacks for processes
struct i386_gate *IDT_ptr;

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
   outportb(0x21,~1);
}



void InitData() {
   int i;
   sys_time = 0;
   //initializing 3 queues
   MyBZero(&run_q,0);
   MyBZero(&none_q,0);
   MyBZero(&sleep_q,0);
   
   for(i = 1 ; i<Q_SIZE;i++){
      pcb[i].state = NONE;
      EnQ(i,&none_q);
      
   }
   CRP = 0;
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
      pcb[CRP].mode = UMODE;
      pcb[CRP].state = RUNNING;
   }
}
int main() {
   InitData(); 		//call Init Data to initialize kernel data
   CreateISR(0);	//call CreateISR(0) to create Idle process (PID 0)
   InitIDT();
   Dispatch(pcb[0].TF_ptr);    // to dispatch/run CRP
   
   return 0;
}
void Kernel(TF_t *TF_ptr) {

   int pid,i;
   char key;
   
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
      default:
         cons_printf("Something went wrong\n");
         breakpoint();
         break;
   }
   
   if (cons_kbhit()) {
      key = cons_getchar(); // saving key pressed into variable key
      switch(key) {
         case 'n':  
            if (none_q.size == 0){   //nothing in none_q
               cons_printf("No more process!\n");
            }else{
            pid = DeQ(&none_q);   //get 1st PID un-used (dequeue none queue)
            CreateISR(pid); 
           
               
               
            } //end else
            break;
         case 't': TerminateISR(); break;   
         case 'b':                                  
            breakpoint();  
            break;
         case 'q':  
            exit(0);                                                 //just do exit(0);
      }// end switch
   }// end if 
   SelectCRP();    //call SelectCRP() to settle/determine for next CRP
   Dispatch(pcb[CRP].TF_ptr);
}

