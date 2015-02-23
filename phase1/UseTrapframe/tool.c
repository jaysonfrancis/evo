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

void EnQ(int pid, q_t *p) {
  
  if (p->size == Q_SIZE) { cons_printf("The q is full"); return; }
  p->q[p->tail] = pid;
  p->tail = (p->tail + 1) % Q_SIZE;
  p->size++;
  return;
  
}

int DeQ(q_t *p) { 

  int pid;
  int tmp;
  
  if (p->size == 0){ return -1; }
  tmp = p->q[p->head];
  p->head = (p->head+1) % Q_SIZE;
  p->size--;
  pid = tmp; 
  return pid;

}


