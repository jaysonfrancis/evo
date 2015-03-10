// proc.c, 159
// processes are here
// Team Evo 

#include "spede.h"   // for IO_DELAY() needed here below
#include "syscall.h"  // Added in Phase 2
#include "extern.h"  // for current_run_pid needed here below
#include "proc.h"     // for Idle, SimpleProc, DispatchProc

void Idle() {
   int i;// Added for seconds 
	for(;;){// Infinity loop
		cons_printf("0 ");   // Print 0 ON host PC
		//Sleep(1);
		for(i=0; i<1666000; i++) IO_DELAY();//busy-loop delay for about 1 sec
   }// End infinite loop 
}

void UserProc() {
   int x,thePID;
   thePID = GetPid();
   x = 4 - (CRP%4); // Calculate proper seconds. Formula taken from 2.html
   for(;;){
		cons_printf("%d ",thePID);	// Print its PID. GetPid() isn't working here?
		Sleep(x);
   } // End infinite loop 
}
 
void Producer(){
	int i;
	 for(;;){
      
      // Wait for product semaphore
      cons_printf("\nProc %d is producing ...\n",CRP);
      product = product+100;
      cons_printf("\n+++ product is now [%d] \n",product);
      // post product semaphore
      for(i=0; i<1666000; i++) IO_DELAY();
   }
} 

void Consumer(){
	int i;
	 for(;;){
      
      // Wait for product semaphore
      cons_printf("\nProc %d is consuming ...\n",CRP);
      product = product-100;
      cons_printf("\n--- product is now [%d] \n",product);
      // post product semaphore
      for(i=0; i<1666000; i++) IO_DELAY();
   }
}

