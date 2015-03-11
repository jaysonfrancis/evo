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

/* 
void Producer() {
   int i, my_pid = GetPid();

   for(;;) {
     SemWait(product_semaphore);

     cons_printf("Proc %d is producing... ", my_pid);
     product += 100;
     cons_printf("+++ product is now %d\n", product);

     SemPost(product_semaphore);

     for(i=0; i<1666000; i++)IO_DELAY();
   }
}
*/

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


/*
void Consumer(){ 
  int i, my_pid = GetPid();

  for(;;){
     SemWait(poduct_semaphore);
     cons_printf("Proc %d is consuming...", my_pid);
     product -= 100;
     cons_printf("--- product is now %d\n", product);

     SemPost(product_semaphore); 
     
     for(i=0; i<1666000; i++) IO_DELAY();
  }
}



*/

// PrintDriver Added in Phase 4

void PrintDriver() {
   int i, code;
   char str[] = "Hello, my Team is called Evo!\n\0";
   char *p;

   print_semaphore = SemGet(0); // should it be -1? depends on IRQISR()

// make sure printer's powered up, cable connected, the following
// statements (until the Sleep call) will reset the printer and the
// first IRQ 7 will occur
   outportb(LPT1_BASE+LPT_CONTROL, PC_SLCTIN); // Printer Control, SeLeCT INterrupt
   code = inportb(LPT1_BASE+LPT_STATUS);
   for(i=0; i<50; i++) IO_DELAY();             // needs delay
   outportb(LPT1_BASE+LPT_CONTROL, PC_INIT|PC_SLCTIN|PC_IRQEN); // IRQ ENable
   Sleep(1);                                   // printer needs time to reset

   while(1) {
      cons_printf("%d ", GetPid());
      Sleep(1);
      if (print_it) { 
         p = str;
         while(*p) {
            outportb(LPT1_BASE+LPT_DATA, *p);
            code = importb(LPT1_BASE+LPT_CONTROL);
            outportb(LPT1_BASE+LPT_control, code|PC_STROBE);
            for(i=0; i<20; i++) IO_DELAY();
            outportb(LPT1_BASE+LPT_CONTROL, code);
            SemWait(print_semaphore);
            p++;
         } // while (*p)
      } // if(print_it)
      print_it = 0;
   } // while(1)

}

