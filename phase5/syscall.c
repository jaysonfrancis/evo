// syscall.c
// collection of syscalls, i.e., API
//Team Evo

#include "syscall.h" // prototype these below

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
   asm("movl %0, %%ebx ;int $51"
      :
      :"g" (semaphoreID)
      :"%ebx");
}

int SemGet(int count) {
  int SemID;
  asm("movl %1,%%eax ; int $52; movl %%ebx, %0;" 
    : "=g" (SemID) 
    : "g" (count)
    : "%eax", "%ebx" ); 
  return SemID;
}

// They both pass the argument as integer (typecast) to the kernel
// Could be wrong.
void MsgSnd(nmg_t *p){ 
  asm("movl %1, %%ebx; int 53; movl %%ecx, %0"
    :
    :"g" ((int)p)
    :"%ebx", %"ecx" );
}

// : "=g" ((msg_t *p))
// ebx is integer... :"g" ((int)p)
//


void MsgRcv(msg_t *p){ 
  asm("movl %1, %%ebx ; int $54; movl %%ecx, %0;"
    :
    :"g" ((int)p)
    :"%ebx", "%ecx");
  }



