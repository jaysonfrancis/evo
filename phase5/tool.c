// tool.c, 159
// Team Evo 


#include "spede.h"
#include "type.h"
#include "tool.h"
#include "extern.h"


void MyBZero(char *p, int size) {
        while(size--){
        	*p++ = (char) 0;

        }
        /*
        int i;
        for (i = 0; i < size;i++){        
                *((char*)p+size) ='\0';
        } */                         
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
   if msg queue q points to is already full {
      cons_printf("Msg queue is already full\n");
      return
   }
   copy what p points to to the msg_q_t that q points to,
   (into the msg array of what q points to, at the slot
   indexed by the tail)

   increment the tail (using q to get to this as well)
   check if it wraps
   increment the size (using q to get to this as well)
}


msg_t *MsgDeQ(msg_q_t *p) {
   if msg queue p points to is empty {
      cons_printf("Msg queue is empty\n");
      return 0 (typical null value)
   }

   the addr of the 1st in the msg array that p points to
   should be the return (put this addr into a separate local
   msg_t pointer first)
   
   decrement the head (using p to get to this as well)
   check if it wraps
   decrement the size (using p to get to this as well)
   
   return that local pointer (of the 1st in msg array p pointed)
}


// assuming src str null-terminated, dest str has space large enough
void MyStrcpy(char *dest, char *src) { // destination 1st, then source

   while, byte by byte, what src points to is not NULL {
      copy character src points to to where dest points to
      advance src (to point to next byte)
      advance dest (to point to next byte)
   }

   add NULL to where dest points to
}



