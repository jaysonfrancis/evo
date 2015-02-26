// proc.c, 159
// processes are here

#include "spede.h"   // for IO_DELAY() needed here below
#include "extern.h"  // for current_run_pid needed here below
#include "proc.h"    // for Idle, SimpleProc, DispatchProc

//Got rid of dispatch for use in trapframe

void Idle() {
	for(;;){
		int i; // We added
		// cons_putchar('0');
		cons_printf("0 "); // print 0 ON PC?
		for (i=0; i<1666000; i++) IO_DELAY(); // busy-loop delay for about 1 sec
	} //end infinite loop
}

void UserProc() {
	int i,x;
	for(;;){
		//cons_printf("%d ", CRP); // print its pid (CRP) on PC
		x=GetPiod();
	cons_printf("%d ", x);
	i = 4 -(X%4);
	Sleep(i);//for (i=0; i<1666000; i++) IO_DELAY(); // busy-loop delay for about 1 sec
	}//end infinite loop
}




