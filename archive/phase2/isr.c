// isr.c, 159
#include "spede.h"
#include "type.h"
#include "isr.h"
#include "tool.h"
#include "extern.h"
#include "proc.h"

void CreateISR(int pid) {
 //  if pid given is not 0 (Idle), enqueue it into run queue
   if (pid != 0) EnQ(pid, &run_q);

 //  PCB of new proc:
    MyBZero(&pcb[pid], sizeof(pcb_t));   

    pcb[pid].mode = UMODE; // Mode is set to UMODE
    pcb[pid].state = RUN; // State is set to RUN
    pcb[pid].runtime = 0;
    pcb[pid].total_runtime = 0; // Both Runtime counts are reset to 0
   
   //Added in trapFrame section
	
	
	MyBZero(stack[pid], STACK_SIZE); // erase stack

	// point to just above stack, then drop by sizeof(TF_t)
	pcb[pid].TF_ptr = (TF_t *)&stack[pid][STACK_SIZE];
	pcb[pid].TF_ptr--;

	// fill out trapframe of this new proc:
	if(pid == 0)
      pcb[pid].TF_ptr->eip = (unsigned int)Idle; // Idle process
	else
      pcb[pid].TF_ptr->eip = (unsigned int)UserProc; // other new process

	pcb[pid].TF_ptr->eflags = EF_DEFAULT_VALUE | EF_INTR;
	pcb[pid].TF_ptr->cs = get_cs();
	pcb[pid].TF_ptr->ds = get_ds();
	pcb[pid].TF_ptr->es = get_es();
	pcb[pid].TF_ptr->fs = get_fs();
	pcb[pid].TF_ptr->gs = get_gs();
}


void TerminateISR() {
   if(CRP < 1) return; //just return if CRP is 0 or -1 (Idle or not given 
   pcb[CRP].state = NONE;//change state of CRP to NONE
   EnQ(CRP,&none_q);//queue it to none queue
   CRP = -1;//set CRP to -1 (none)

}        

void TimerISR() {
        outportb(0x20, 0x60); // Timer interrupt dismissed 
	
	if(CRP<=0) return; //just return if CRP is Idle (0) or less (-1)
	
        pcb[CRP].runtime++;//upcount the runtime of CRP
	
        if (pcb[CRP].runtime == TIME_LIMIT){ //if the runtime of CRP reaches TIME_LIMIT
	  pcb[CRP].total_runtime += pcb[CRP].runtime;//sum up runtime to the total runtime of CRP
	  pcb[CRP].state=RUN;//change its state to RUN
	  EnQ(CRP,&run_q);//queue it to run queue
	  CRP=-1; //reset CRP (to -1, means none) 

	//Phase 2
	sys_time++;
	chksleepQ();      
}

void chksleepQ(){
  int i;
  for (i=0; i<sizeof(&sleep_q); i++){
    if (pcb[sleep_q.q[i]].waketime == sys_time){
	pcb[sleep_q.q[i]].state = RUN;
        EnQ(sleep_q.q[i], &run_q);
	}	
  }
}

void GetPidISR() { // Phase 2
  outportb(0x20, 0x60); // ? 
  pcb[CRP].TF_ptr->ebx = CRP;
  GetPid();
  //pcb[CRP].TF_ptr = pid; 
  
  //return pcb[CRP].TF_ptr;
  return;
}

void SleepISR(int sec) { // Phsae 2
  //int sec;
  outportb(0x20, 0x60);
  //sec = pcb[CRP].TF_ptr->ebx; //
  
  pcb[CRP].waketime = (sys_time + (100 * sec));
  
  EnQ(CRP, &sleep_q);
  pcb[CRP].state=SLEEP;
  CRP=-1;
  return;
}



}
