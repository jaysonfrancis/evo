// syscall.c
//collection of syscalls, i.e., API

#include "syscall.h" //prototype these below

int GetPid(){ //programed to entry 48 in IDT
	int pid;
	
	//assembly code initialization
	asm("int $48; movl %%ebx, %0" //CPU inst call intterupt 48 (GETPID_INTR)
	:"=g" (pid) // output line for asm().
	:          // no input into asm()
	:"%ebx");  // push/pop before/after asm()
	
	return pid;
}

void Sleep(int sec){ //coded to entry 49 in IDT
	/* TimerEntry in entry.S
	// must push 32, then 48, then 49
	*/
	asm("movl %0, %%ebx; int $49"
	: //no output line for asm()
	:"g" (sec) //input line for asm()
	:"%ebx" // what register to push/pop before/after asm()
	);
}
