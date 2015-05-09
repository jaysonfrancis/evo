// syscall.c
// collection of syscalls, i.e., API
//Team Evo

#include "syscall.h" // prototype these below
#include "type.h"

int GetPid() {
   int pid;

   asm("int $48; movl %%ebx, %0" 
       : "=g"  (pid)             
       :                         
       : "%ebx");              
    
   return pid;
}

void Sleep(int sec) {
    asm("movl %0, %%ebx ;int $49"
        :
        :"g" (sec)
        :"%ebx");
}

void SemWait(int semaphoreID){
   asm("movl %0, %%ebx ;int $50"
      :
      :"g" (semaphoreID)
      :"%ebx");
}

void SemPost(int semaphoreID){
   asm("movl %0, %%ebx ; int $51"
      :
      :"g" (semaphoreID)
      :"%ebx");
}

int SemGet(int count) {
  int SemID;
  asm("movl %1,%%ebx ; int $52; movl %%ecx, %0;" 
    : "=g" (SemID) 
    : "g" (count)
    : "%ebx", "%ecx" ); 
  return SemID;
}

// They both pass the argument as integer (typecast) to the kernel
// Could be wrong.
void MsgSnd(msg_t *p){ 
  asm("movl %0, %%ebx; int $53"
    :
    :"g" ((int)p)
    :"%ebx" );
}



void MsgRcv(msg_t *p){ 
  asm("movl %0, %%ebx ; int $54"
    :
    :"g" ((int)p)
    :"%ebx");
  }

void TipIRQ3(){
  asm("int $35");
}

void Fork(char *){
   asm("movl %0, %%ebx; int $55" 
       : 
       : "g" ((int) message)                        // no input into asm()
       : "ebx", "eax");
}

int Wait(int *){
   asm("movl %0, %%ebx; int $56" 
       : 
       : "g" ((int) message)     
       : "ebx", "eax"); 
}

void Exit(int){
   asm("movl %0, %%ebx; int $57" 
       : 
       : "g" ((int) message)                        // no input into asm()
       : "ebx", "eax"); 
}



