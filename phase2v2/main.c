// CpE/CSc159
// UseTrapframe/main.txt
// Group name: Evo.
// Student Name: Jayson Francis & Aaron Blancaflor

#include "spede.h"
#include "type.h"
#include "main.h"
#include "isr.h"
#include "proc.h"
#include "tool.h"
#include "entry.h"
#include "TF.h"

int CRP, sys_time;
q_t run_q, none_q, sleep_q;
pcb_t pcb[MAX_PROC];
char stack[MAX_PROC][STACK_SIZE];
struct i386_gate *IDT_ptr;


void SetEntry(int entry_num, func_ptr_t func_ptr){
  struct i386_gate *gateptr = &IDT_ptr[entry_num];
  fill_gate(gateptr, (int)func_ptr, get_cs(), ACC_INTR_GATE, 0);
}

void InitData(){
	int i;
	sys_time = 0;
   //initializing 3 queues
	MyBZero(&run_q, 0);
	MyBZero(&none_q, 0);
	MyBZero(&sleep_q, 0); // Phase 2

   for(i=1; i<20; i++){
	EnQ(i, &none_q);//queue PID's 1-19 into none_q
	pcb[i].state = NONE; //set state to NONE in all unused PCB[1-19]
   }//end for loop
   CRP=0;

}

void SelectCRP(){
	if(CRP>0) return;

	if(CRP==0) pcb[CRP].state=RUN;

	if(run_q.size == 0){
		CRP = 0;
	}else{
		CRP = DeQ(&run_q);
		pcb[CRP].mode = UMODE;
		pcb[CRP].state = RUNNING;
	}
}


void InitIDT(){
  IDT_ptr = get_idt_base(); //locate IDT
  SetEntry(32, TimerEntry);//fill out IDT timer entry, set entry 32
  SetEntry(48, GetPidEntry); // Added in Phase 2
  SetEntry(49, SleepEntry); // Added in Phase 2
  outportb(0x21, ~1);//program PIC mask
}

int main(){
   InitData(); //call InitData() to initialize kernel data structure
   CreateISR(0); //call CreateISR() to create Idle proc. create with 0
   InitIDT();  //call (new) InitIDT() to set up timer (from timer lab)
   Dispatch(pcb[0].TF_ptr);//call Dispatch(pcb[...) to load Idle proc to run pcb[0].TF_ptr

   	return 0;
   }

   void Kernel(TF_t *TF_ptr){
   	char key;
   	int pid;
    pcb[CRP].TF_ptr = TF_ptr;

    pcb[CRP].mode = KMODE; //change to kernel mode for CRP - change to kmode
    //save TF_ptr to PCB of CRP 
   
   switch(TF_ptr->intr_num){ 
      case TIMER_INTR://if it's TIMER_INTR:
        TimerISR(); //call TimerISR()
        break;
      case GETPID_INTR: // Phase 2
        //GetPid();
      GetPidISR();
      break;
      case SLEEP_INTR:  // Phase 2
	       //Sleep(pcb[CRP].wake_time); // ? 
      SleepISR(TF_ptr->ebx);
      default://default:
         cons_printf("Something went wrong"); //PANIC! msg and break into GDB
         breakpoint();
         break;

   }//end switch

  if (cons_kbhit()){ // if key stoke is detected
	 key = cons_getchar(); //determining what key was pressed

	 switch(key){
	 	case 'n':
	 	if(none_q.size==0){cons_printf("No more processes!\n");}
	 	else{
	 		pid = DeQ(&none_q);
	 		CreateISR(pid);
	   }//end else
	   break;
	   case 't':
	   TerminateISR();
	   break;
	   case 'b':
	   breakpoint();
	   break;
	   case 'q':
	   exit(0);
	 }//end switch statement 	
  }//end if some key pressed

   SelectCRP();//call SelectCRP() to select process to run
   Dispatch(pcb[CRP].TF_ptr);//call Dispatch(pcb[CRP].TF_ptr) to load it and run
}
