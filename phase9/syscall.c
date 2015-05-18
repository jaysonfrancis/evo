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

void Fork(char *message){
   asm("movl %0, %%ebx; int $55" 
       : 
       : "g" ((int) message)                        // no input into asm()
       : "ebx", "ebx");
}

int Wait(int *message){
  int pid;
  asm("movl %1, %%ebx; int $56; movl %%ecx, %0;"
  :"=g" (pid)
  :"g" ((int)exit_num_ptr)
  :"%ebx","%ecx");
    return pid;
}

void Exit(int message){
  asm("movl %0, %%ebx; int $57"
  :
  :"g" (exit_num)
  :"%ebx");
}

