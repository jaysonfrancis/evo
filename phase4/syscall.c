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
  int semaphore_ID;
  asm("movl %1,%%ebx ; int $52; movl %%ecx, %0;" 
    : "=g" (semaphore_ID) 
    : "g" (count)
    : "%ebx", "%ecx" ); 
  return sempahore_ID;
}