// proc.c, 159
// processes are here

#include "spede.h"   // for IO_DELAY() needed here below
#include "extern.h"  // for current_run_pid needed here below
#include "proc.h"    // for Idle, SimpleProc, DispatchProc
#include "syscall.h"

void Idle() {
   int i;//seconds;
   for(;;){
   cons_printf("0");//print 0 on PC
   //seconds= 4 - (CRP%4);
   //Sleep(seconds);
   for(i=0; i<1666000; i++) IO_DELAY();//busy-loop delay for about 1 sec
   } 
}

void UserProc() {
  // int i;
   int seconds;
   for(;;){
   cons_printf("%d",CRP);//print its pid (CRP) on PC
   GetPid();
   seconds= 4 - (CRP%4);
   Sleep(seconds);
   //printf("seconds %d", seconds);
   // Sleep(seconds);
   //for(i=0; i<1666000; i++) IO_DELAY();//busy-loop delay for about 1 sec
   }
   
}
 
void Producer(){
   int i;
   while(1){
      SemWait(semaphoreID);// Wait for product semaphore
      cons_printf("\n %d is producing ...\n",CRP);
      product += 100;
      cons_printf("\n>>> product is now %d \n",CRP);
      SemPost(semaphoreID);// post product semaphore
      for(i=0; i<1666000; i++) IO_DELAY();
   }
}

void Consumer(){
   int i;
   while(1){
      SemWait(semaphoreID);// Wait for product semaphore
      cons_printf("\n %d is consuming ...\n",CRP);
      product -= 100;
      cons_printf("\n<<< product is now %d \n",CRP);
      SemPost(semaphoreID);// post product semaphore
      for(i=0; i<1666000; i++) IO_DELAY();
   }
}
