// isr.c, 159

#include "spede.h"
#include "type.h"
#include "isr.h"
#include "tool.h"
#include "extern.h"
#include "proc.h"
#include "syscall.h"

int wakeup;
int wp; 
void CreateISR(int pid) {

 if(pid !=0 ){ EnQ(pid,&run_q); }

      // PCB of new proc:
 pcb[pid].mode = UMODE;
 pcb[pid].state= RUN;
 pcb[pid].runtime = 0;
 pcb[pid].total_runtime = 0;

      // Added in Phase 1 (TrapFrame)

      MyBzero(stack[pid], STACK_SIZE); // Erase the current stack

      pcb[pid].TF_ptr = (TF_t *)&stack[pid][STACK_SIZE];
      pcb[pid].TF_ptr--;

      if(pid == 0) pcb[pid].TF_ptr->eip = (unsigned int)Idle; // Idle process
      else pcb[pid].TF_ptr->eip = (unsigned int)UserProc; // other new process
      
      pcb[pid].TF_ptr->eflags = EF_DEFAULT_VALUE | EF_INTR;
      pcb[pid].TF_ptr->cs = get_cs();
      pcb[pid].TF_ptr->ds = get_ds();
      pcb[pid].TF_ptr->es = get_es();
      pcb[pid].TF_ptr->fs = get_fs();
      pcb[pid].TF_ptr->gs = get_gs();
    }

    void TerminateISR() {
      if (CRP <=0 ){ return; }            // Return if CRP is 0 or -1

      pcb[CRP].state = NONE;              // Changing state of CRP to NONE
      EnQ(CRP,&none_q);                   // Queue CRP into the NONE queue
      CRP = -1;                           // Set CRP to -1 (none)

    }    

    void GetPidISR(){

      outportb(0x20,0x60);                // Release
      pcb[CRP].TF_ptr->ebx = CRP;         
     // GetPid(); //maybe change this back

      return;
    }

    // Added in Phase 2
   void chksleepQ(){
    while(sleep_q.size !=0 && pcb[sleep_q.q[sleep_q.head]].wake_time <= sys_time){ 
      wakeup = DeQ(&sleep_q);
      pcb[wakeup].state=RUN;
      EnQ(wakeup,&run_q);
    }
   }

    void TimerISR() {
     outportb(0x20,0x60);                 // Release

     pcb[CRP].runtime++;
     sys_time++;
     //pcb[CRP].runtime++;                  // Increment Runtime
     //sys_time++;                          // Increment Systime

     chksleepQ();                         // Added method chksleepq()''
     if (CRP<=0)return; // return if CRP is 0
     if(pcb[CRP].runtime == TIME_LIMIT){

      pcb[CRP].total_runtime=pcb[CRP].runtime + pcb[CRP].total_runtime;
      pcb[CRP].runtime=0;
      pcb[CRP].state = RUN;
      EnQ(CRP,&run_q);
      CRP = -1;

    }
  }

// Added in Phase 2
void SleepISR(int seconds){
  outportb(0x20,0x60);
  wp= sys_time+(100*seconds);
  pcb[CRP].wake_time=wp;
  EnQ(CRP,&sleep_q);
  pcb[CRP].state=SLEEP;
  CRP=-1;
  return;
  
}
