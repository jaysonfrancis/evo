// CpE/CSc159
// UseTrapframe/main.txt
// Group name: Evo
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


//InitData() still same as PureSimulation
void InitData(){
   int i;
   sys_time = 0;
   //initializing 2 queues
   MyBZero(&run_q, sizeof(run_q));
   MyBZero(&none_q, sizeof(none_q));
   MyBZero(&sleep_q, sizeof(sleep_q)); // Phase 2

   for(i=1; i<20; i++){
	EnQ(i, &none_q);//queue PID's 1-19 into none_q
	pcb[i].state = NONE; //set state to NONE in all unused PCB[1-19]
   }//end for loop
   CRP=0;
}

//SelectCRP() still the same as PureSimulation
void SelectCRP(){
    if(CRP>0) return; //simply return  if CRP is greater than 0
    
    if (CRP == 0) pcb[CRP].state=RUN;

    if(run_q.size == 0)
	CRP=0;
    else
	CRP = DeQ(&run_q);

    pcb[CRP].mode = UMODE;
    pcb[CRP].state = RUN;
}

//SetEntry() needed from timer lab
void SetEntry(int entry_num, func_ptr_t func_ptr){
    struct i386_gate *gateptr = &IDT_ptr[entry_num];
    fill_gate(gateptr, (int)func_ptr, get_cs(), ACC_INTR_GATE, 0);
}


void InitIDT(){ //InitIDT() is new to code, containing 3 statements from timer lab
  IDT_ptr = get_idt_base(); //locate IDT
  SetEntry(32, TimerEntry);//fill out IDT timer entry, set entry 32
  SetEntry(48, GetPidEntry); // Added in Phase 2
  SetEntry(49, SleepEntry); // Added in Phase 2
  outportb(0x21, ~1);//program PIC mask
  //(but NO "sti")
}

int main() {
   InitData(); //call InitData() to initialize kernel data structure
   InitIDT();  //call (new) InitIDT() to set up timer (from timer lab)
   CreateISR(0); //call CreateISR() to create Idle proc. create with 0
   Dispatch(pcb[0].TF_ptr);//call Dispatch(pcb[...) to load Idle proc to run pcb[0].TF_ptr

   return 0; // never reaches this.
} // main ends

void Kernel(TF_t *TF_ptr) {
  char key;
  int pid;
   pcb[CRP].mode = KMODE; //change to kernel mode for CRP - change to kmode
   //save TF_ptr to PCB of CRP 
  pcb[CRP].TF_ptr = TF_ptr;

  switch(TF_ptr->intr_num){ 
      case TIMER_INTR://if it's TIMER_INTR:
        TimerISR(); //call TimerISR()
	break;
      case GETPID_INTR: // Phase 2
        GetPid();
	break;
      case SLEEP_INTR;  // Phase 2
	Sleep(pcb[CRP].wake_time); // ? 
      default://default:
         cons_printf("Something went wrong"); //PANIC! msg and break into GDB
	 exit(0);

   }//end switch

   if (cons_kbhit()){ // if key stoke is detected
      //char key;
      //int pid;
	key = cons_getchar(); //determining what key was pressed

	switch(key){
	case 'n':
	   if(none_q.size==0)cons_printf("No more processes!\n");
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
