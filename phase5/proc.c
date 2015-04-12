// proc.c, 159
// processes are here
// Team Evo 

#include "spede.h"   // for IO_DELAY() needed here below
#include "syscall.h"  // Added in Phase 2
#include "tool.h"
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
   int thePID;

   // Added in the loop. ? thePID = GetPid();
   //x = 4 - (CRP%4); // Calculate proper seconds. Formula taken from 2.html
   
   for(;;){
    thePID = GetPid(); 
    cons_printf("%d ", thePID); // Print its PID. GetPid() isn't working here?
    Sleep(4 - thePID%4);
   
   } // End infinite loop 
}
 

// PrintDriver Added in Phase 4

void PrintDriver() {
   int i, code, thePID;
   msg_t my_msg;
   char str[] = "Hello, my Team is called Evo!\n\0";
   char *p;

   print_semaphore = SemGet(-1); // should it be -1? depends on IRQISR()

// make sure printer's powered up, cable connected, the following
// statements (until the Sleep call) will reset the printer and the
// first IRQ 7 will occur

   outportb((LPT1_BASE+LPT_CONTROL), PC_SLCTIN); // Printer Control, SeLeCT INterrupt
   code = inportb(LPT1_BASE+LPT_STATUS);
   for(i=0; i<50; i++) IO_DELAY();             // needs delay
   outportb(LPT1_BASE+LPT_CONTROL, PC_INIT|PC_SLCTIN|PC_IRQEN); // IRQ ENable
   Sleep(1);                                   // printer needs time to reset

   while(1) {
      thePID = GetPid();
      cons_printf("%d ", thePID);

      MsgRcv(&my_msg);
      p = my_msg.data;
      while (*p){
          outportb(LPT1_BASE+LPT_DATA, *p);
            code = importb(LPT1_BASE+LPT_CONTROL);
            outportb(LPT1_BASE+LPT_CONTROL, code|PC_STROBE);
            for(i=0; i<20; i++) IO_DELAY();
            outportb(LPT1_BASE+LPT_CONTROL, code);
            SemWait(print_semaphore);
            p++;
    }
   } 
  }




void Init(){
  int key;
  char str[] ="Hello, Evo team for 159\n";
  msg_t msg;
  msg.recipient =2;
  MyStrcpy(msg.data, str);

  while(1){
    cons_printf(" %2x ", GetPid());
    Sleep(1);

    if (cons_kbhit()){
      key = cons_getchar();
      switch(key){
        case 'b': breakpoint(); break;
        case 'q': exit(0);
        case 'p':
            MsgSnd(&msg);
            break;
        default:
            cons_printf("Error - No command\n");
            break;
      }
    }
  }
}



