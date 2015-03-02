// tool.c, 159
// Team Evo 


#include "spede.h"
#include "type.h"
#include "extern.h"


void MyBzero(void *s, int n) {
	int i;
	
	for (i = 0; i < n; i++) {
	  *((char*)s+n) = '\0';
	}
	

//	for (i=0; i<size; i++){
 //   *p = (char) 0;
 //   p++;
//   }
   
   
}

// Edited on Feb 28th @ 11:32PM
void MyBZero(q_t *p, int size) {
	p->size = size;
	p->head = size;
	p->tail = size;
}

void EnQ(int pid,  q_t *p) {

	if (p->size == Q_SIZE){ printf("\nThe queue is full"); return; } // If none available
	else

	p->q[p->tail]= pid;
	p->tail ++;
	
	if (p->tail >= Q_SIZE) { p->tail = 0; }
	
	p->size ++;
}

int DeQ(q_t *p) { 
	int pid;
	
	if (p->size == 0){ printf("The queue is empty\n"); return -1; } // Nothing to dequeue
	else
	
	pid=p->q[p->head];
	p->head++;
	
	if(p->head==Q_SIZE){ p->head=0; }
	
	p->size--;

	return pid;
}

