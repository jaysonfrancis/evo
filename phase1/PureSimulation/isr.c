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

}


void TerminateISR() {
   if(CRP < 1) return;//just return if CRP is 0 or -1 (Idle or not given)
      
   pcb[CRP].state = NONE;//change state of CRP to NONE
   EnQ(CRP,&none_q);//queue it to none queue
   CRP = -1;//set CRP to -1 (none)
}        

void TimerISR() {
        if(CRP<=0) return; //just return if CRP is Idle (0) or less (-1)
        pcb[CRP].runtime++;//upcount the runtime of CRP
        if (pcb[CRP].runtime == TIME_LIMIT){ //if the runtime of CRP reaches TIME_LIMIT
	 // ??//(need to rotate to next PID in run queue)
	pcb[CRP].total_runtime += pcb[CRP].runtime;//sum up runtime to the total runtime of CRP
	pcb[CRP].state=RUN;//change its state to RUN
	EnQ(CRP,&run_q);//queue it to run queue
	CRP=-1; //reset CRP (to -1, means none)     
	   }
       
}
