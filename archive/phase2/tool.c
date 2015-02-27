// tool.c, 159

#include "spede.h"
#include "type.h"
#include "extern.h"

void MyBZero(char *p, int size) {

  int i;
  for (i=0; i<size; i++){
    *p = (char) 0;
    p++;
   }
 
}

//void checksleepQ(){
// Key is to look at the size


void EnQ(int pid, q_t *p) {
  
  if (p->size == Q_SIZE) { cons_printf("The q is full"); return; }
  else
  p->q[p->tail] = pid;
  p->tail = (p->tail + 1) % Q_SIZE;
  
  //new
  if (p->tail >= Q_SIZE){ p->tail = 0; }

  p->size++;
  return;
  
}

int DeQ(q_t *p) { 

  int pid;
  int tmp;
  
  if (p->size == 0){ return -1; }else
  tmp = p->q[p->head];
  // New
  p->head++;
  if (p->head==Q_SIZE){ p-> head=0; }

  // New - p->head = (p->head+1) % Q_SIZE;
  p->size--;
  //pid = tmp; 
  return pid;

}


