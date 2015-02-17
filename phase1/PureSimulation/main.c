// main.c, 159
// simulated kernel
// here is a change 
// Team Name: evo  (Members: Aaron Blancaflor and Jayson Francis)

#include "spede.h"      // spede stuff
#include "main.h"       // main stuff
#include "isr.h"        // ISR's
#include "tool.h"       // handy functions
#include "proc.h"       // processes such as Init()
#include "type.h"       // processes such as Init()

// kernel data structure:
int CRP;                // current running PID, -1 means no process
q_t run_q, none_q;      // processes ready to run and not used
pcb_t pcb[MAX_PROC];    // process table
char stack[MAX_PROC][STACK_SIZE]; // run-time stacks for processes

int main() {
   InitData(); // call InitData() to initialize kernel data
   CreateISR(0); // call CreateISR(0) to create Idle process (PID 0)

   while(true){ //infinite loop: {      // alter 2 things below
     Dispatch(); // call Dispatch()    // to dispatch/run CRP
     Kernel(); //  call Kernel()      // for kernel control
   }

   return 0;
}

void InitData() {
   MyBZero(&run_q, sizeof(run_q)); // initialize 2 queues (use MyBzero() you code in tool.c/.h)
   MyBZero(&none_q, sizeof(none_q));

   queue PID's 1~19 (skip 0) into none_q (un-used PID's)
   set state to NONE in all un-used pcb[1~19]
   CRP = 0; // set CRP to 0 (Idle proc ID)
}

void SelectCRP() {       // select which PID to be new CRP
   if(CRP>0) return 0;// simply return if CRP is greater than 0 (already good one selected)
   //else if (CRP == -1 | CRP == 0) { // (continue only when CRP is Idle or none (0 or -1)

   if (CRP == 0) pcb[CRP].state = RUN; // if it's 0 (Idle), change its state in PCB to RUN

   if (sizeof(run_q) == 0) // if no processes to run (check size in run queue against zero)
      CRP = 0; // set CRP to 0 (at least we can run Idle proc)
   else
      CRP = DeQ(run_q); //set CRP to first in run queue (dequeue it)

   pcb[CRP].mode = UMODE; // change mode in PCB of CRP to UMODE
   pcb[CRP].state = RUN; // change state in PCB of CRP to RUNNING
}

void Kernel(TF_T *TF_ptr) {
   int pid;
   char key;

   switch(TF_ptr->intr_num){ //change state in PCB of CRP to kernel mode
   case TIME_INTR:
    TimerISR(); // call TimerISR() to service timer interrupt as it just occured
    break;
   }
  

  if (cons_kbhit()){ //  check if key pressed on PC {
   key = cons_getchar(); //   read in pressed key
      switch(key) {
        case 'n':  // if 'n'
            if(none_q.size<=0) // if no processes left in none queue
             cons_printf("No more process!\n");  // "No more process!\n" (msg on target PC)
            else
               pid = DeQ(none_q); // get 1st PID un-used (dequeue none queue)
	       CreateISR(pid); // call CreateISR() with it to create new process
            break; 
	 case 't': // if 't'
            TerminateISR();  //call TerminateISR() to terminate CRP
            break;
	 case 'b': // if 'b'
            break; // just do breakpoint(); // this goes back to GDB prompt
	 case 'q': // if 'q'
            exit(0); // just do exit(0);
     } // end switch
   } // end if some key pressed

   SelectCRP(); // call SelectCRP() to settle/determine for next CRP
}

