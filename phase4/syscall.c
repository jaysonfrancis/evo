// syscall.c
// collection of syscalls, i.e., API
//Team Evo

#include "syscall.h" // prototype these below

int GetPid(void) {
 int pid;

   asm("int $48; movl %%ebx, %0" // CPU inst
       : "=g"  (pid)             // 1 output from asm() 
       :                         // no input into asm()
       : "%ebx");                // push/pop before/after asm()
   
   return pid;
 }

 void Sleep(int sec) {
  asm("movl %0, %%eax ;int $49"
    :
    :"g" (sec)
    :"eax");
}

void SemWait(int SemID){
  asm("movl %0, %%eax ;int $50"
    :
    :"g" (SemID)
    :"ebx"  
    );
}

void SemPost(int SemID){
  asm ("movl %0, %%eax ;int $51"
    :
    :"g" (SemID)
    :"ebx");
}


int SemGet(int count){
  int semaphore_id;

  asm("movl %1, %%eax; int $52; movl %%ebx, %0"
    : "=g" (semaphore_id)
    : "g" (count)
    : "ebx", "eax");

  return semaphore_id;

}