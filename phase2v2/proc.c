// proc.c, 159
// processes are here

#include "spede.h"   // for IO_DELAY() needed here below
#include "extern.h"  // for current_run_pid needed here below
#include "proc.h"    // for Idle, SimpleProc, DispatchProc
#include "syscall.h" // Added in Phase 2

void Idle() {
	int i; // We added
	for(;;){
		cons_printf("0 "); // print 0 ON PC?
		for (i=0; i<1666000; i++) IO_DELAY(); // busy-loop delay for about 1 sec
	} //end infinite loop
}

void UserProc() {
	int s,x;
	for(;;){
		x=GetPid();
		cons_printf("%d ", x);
		s = 4 -(x%4);
	    Sleep(s);//for (i=0; i<1666000; i++) IO_DELAY(); // busy-loop delay for about 1 sec
	}//end infinite loop
}