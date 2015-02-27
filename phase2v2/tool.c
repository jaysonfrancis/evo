// tool.c, 159

#include "spede.h"
#include "type.h"
#include "extern.h"
/*
void MyBZero(char *p, int size) {

  int i;
  for (i=0; i<size; i++){
    *p = (char) 0;
    p++;
  }

}
*/
void MyBZero(q_t *p, int size) {
  p->size = size;
  p->head = size;
  p->tail = size;
}

void MyBzero(void *s, int n) {
  int i;
  for (i = 0; i < n; i++) {
  *((char*)s+n) = '\0';
  }
}

void EnQ(int pid, q_t *p) {

  if (p->size == Q_SIZE) { cons_printf("The q is full"); return; }
  else
    p->q[p->tail] = pid;
    p->tail++;
  

  
  //new
  if (p->tail >= Q_SIZE){ p->tail = 0; }

  p->size++;
  //return;
  
}

int DeQ(q_t *p) { 

  int pid;

  
  if (p->size == 0){ return -1; }else
  pid=p->q[p->head];
  p->head++;
  
  if(p->head==Q_SIZE){
    p->head=0;
  }
  p->size--;
  return pid;
}


