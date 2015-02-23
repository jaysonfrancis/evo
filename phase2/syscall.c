// syscall.c
//collection of syscalls, i.e., API

#include "syscall.h" //prototype these below

int GetPid(){
	int pid;
	
	//assembly code initialization
	asm("int $48; movl %%ebx, %0" //CPU inst
	:"g" (pid) // output line for asm().
	:          // no input into asm()
	:"%ebx");  // push/pop before/after asm()
	
	return pid;
}

void Sleep(int sec){
	//??? need to complete the code
}