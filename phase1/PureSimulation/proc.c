// proc.c, 159
// processes are here

#include "spede.h"   // for IO_DELAY() needed here below
#include "extern.h"  // for current_run_pid needed here below
#include "proc.h"    // for Idle, SimpleProc, DispatchProc

void Dispatch() {
   if(CRP==) //if CRP is 0:
      Idle(); // call Idle() as proc run
   else // (all other processes)
      UserProc(); // call UserProc() as proc run
}

void Idle() {
   int i; // We added
   cons_putchar('0'); // print 0 ON PC?
   for (i=0; i<100000; i++) IO_DELAY(); // busy-loop delay for about 1 sec
}

void UserProc() {
   int i; // We added
   cons_printf("%d ", CRP); // print its pid (CRP) on PC
   for (i=0; i<100000; i++) IO_DELAY(); // busy-loop delay for about 1 sec
}



