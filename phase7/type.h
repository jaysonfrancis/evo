// type.h, 159
// Team Evo

#ifndef _TYPE_H_
#define _TYPE_H_

#include "TF.h"

#define TIME_LIMIT 300       // max timer count to run
#define MAX_PROC 20          // max number of processes
#define Q_SIZE 20            // queuing capacity
#define STACK_SIZE 4096      // process stack in bytes

// this is the same as constants defined: KMODE=0, UMODE=1
typedef enum {KMODE, UMODE} mode_t;
typedef enum {NONE, RUNNING, RUN, SLEEP, WAIT, ZOMBIE} state_t;

typedef struct {             // PCB describes proc image
   mode_t mode;              // process privilege mode
   state_t state;            // state of process
   int runtime;              // run time since dispatched
   int total_runtime;        // total run time since created
   TF_t *TF_ptr;             // points to TF in stack
   int wake_time;            // amount of time to wait before wait
} pcb_t;

typedef struct {             // proc queue type
   int head, tail, size;     // where head and tail are, and current size
   int q[Q_SIZE];            // indices into q[] array to place or get element
} q_t;

typedef struct{
	int count;
	q_t wait_q;
} semaphore_t;

typedef void (* func_ptr_t)(); // void-returning function pointer type

// Added in Phase 5 (message passing)

typedef struct {
   int sender,                // Sender
       recipient,             // Recipient
       time_stamp;            // Time sent
       code;
   int number[3];
   char data[101];            // Data bytes
} msg_t;

typedef struct {
   msg_t msg[Q_SIZE];
   int head, tail, size;
} msg_q_t;

typedef struct {
   msg_q_t msg_q;
   q_t wait_q;
} mbox_t;


// Added in Phase 6 (Bidirectional Device Driver)

typedef struct {
  q_t TX_q,   // transit to terminal
      RX_q,   // receieve from terminal
      echo_q;  // echo back to terminal
  int TX_sem,  // transmit space available count
      RX_sem,  // recieve data (arrived) count
      echo,   // echo back to terminal or not (typing)
      TX_extra; // if 1, TXRDY event occured but echo_q and TX_q were empty
} terminal_t

#endif _TYPE_H_
