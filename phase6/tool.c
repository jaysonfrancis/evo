// tool.c, 159
// Team Evo 


#include "spede.h"
#include "type.h"
#include "extern.h"


void MyBZero(char *p, int size) {
        while(size--){
          *p++ = (char) 0;

        }
                        
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


void MsgEnQ(msg_t *p, msg_q_t *q) {
	if (q->size == Q_SIZE){
		cons_printf("Msg queue is already full!\n");
		return;
	}
	q->msg[q->tail]=*p;
	q->size++;
	q->tail++;

	if(q->tail == Q_SIZE){
		q->tail = 0;
	}

}



msg_t *MsgDeQ(msg_q_t *p) {

	//separate local pointer
	msg_t *localmsg;

   if (p->size == 0){
      cons_printf("Msg queue is empty\n");
      return '\0';
   }
   localmsg = p->msg;
   p->head--;
   if(p->head == Q_SIZE){
   		p->head = 0;
   }
   p->size--;
   return localmsg;

}


// assuming src str null-terminated, dest str has space large enough
void MyStrcpy(char *dest, char *src) { // destination 1st, then source
	while (*src){
		*dest = *src;
		src++;
		dest++;
	}
	*dest='\0';

}


int MyStrcmp(char *, char *){}

int MyStrlen(char *){}





