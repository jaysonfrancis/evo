// isr.c, 159
// Team Evo
#include "spede.h"
#include "type.h"
#include "isr.h"
#include "tool.h"
#include "extern.h"
#include "proc.h"
#include "syscall.h"
#include "FileMgr.h"
int PIDtoWake;
int wakeLength;
void CreateISR(int pid) {
   if(pid !=0 ){//if pid given is not 0 (Idle), enqueue it into run queue
      EnQ(pid,&run_q);
   }
      // PCB of new proc:
      pcb[pid].mode = UMODE;//mode is set to UMODE
      pcb[pid].state= RUN;//state is set to RUN
      pcb[pid].runtime = 0;// both runtime counts are reset to 0
      pcb[pid].total_runtime = 0;// both runtime counts are reset to 0
      
      MyBZero((char*) &mbox[pid], sizeof(mbox_t));
      MyBZero(stack[pid], STACK_SIZE); // erase stack

      // point to just above stack, then drop by sizeof(TF_t)
      pcb[pid].TF_ptr = (TF_t *)&stack[pid][STACK_SIZE];
      pcb[pid].TF_ptr--;
      // fill out trapframe of this new proc:
      if(pid==0) pcb[pid].TF_ptr->eip = (unsigned int)Idle; // Idle process
      if(pid==2) pcb[pid].TF_ptr->eip = (unsigned int)PrintDriver; // other new process
      if(pid==1) pcb[pid].TF_ptr->eip = (unsigned int)Init;
      if(pid==3) pcb[pid].TF_ptr->eip = (unsigned int) Shell;
      if(pid==4) pcb[pid].TF_ptr->eip = (unsigned int) STDIN;
      if(pid==5) pcb[pid].TF_ptr->eip = (unsigned int) STDOUT; 
      if(pid==6) pcb[pid].TF_ptr->eip = (unsigned int) FileMgr;
      // Userproc not used for phase5
      if(pid<0 || pid>6) pcb[pid].TF_ptr->eip = (unsigned int)UserProc;


      pcb[pid].TF_ptr->eflags = EF_DEFAULT_VALUE | EF_INTR;
      pcb[pid].TF_ptr->cs = get_cs();
      pcb[pid].TF_ptr->ds = get_ds();
      pcb[pid].TF_ptr->es = get_es();
      pcb[pid].TF_ptr->fs = get_fs();
      pcb[pid].TF_ptr->gs = get_gs();
      
   
}

void TerminateISR() {
   //just return if CRP is 0 or -1 (Idle or not given)
    
    if (CRP <=0 ){
      return;
    }  
   //change state of CRP to NONE
   pcb[CRP].state = NONE;
   //queue it to none queue
   EnQ(CRP,&none_q);                             //STILL ERROR HERE
   //set CRP to -1 (none)
   CRP = -1;
 //  return 0;
}    

void GetPidISR(){
    pcb[CRP].TF_ptr->ebx = CRP;
  return;
}

void TimerISR() {
   outportb(0x20,0x60);
   
  
   //upcount the runtime of CRP and system time
   pcb[CRP].runtime++;
   sys_time++;

   /////////////////////////////////////////////////////////////////////////////////////////
   /*
     Scanning the sleep queue is not implemented correctly. (-1.5)
     If sleep_q.size > 1 and pcb[sleep_q.q[sleep_q.head]].wake_time > sys_time,
     it will exit the while loop without checking if any other pcb's on the 
     sleep_q should be enqueued onto the run_q. Another pcb on the sleep_q in a 
     different position then the sleep_q.head position, may have 
     wake_time <= sys_time.
    */

   while(sleep_q.size != 0 && pcb[sleep_q.q[sleep_q.head]].wake_time <= sys_time){
     //int PIDtoWake;
     PIDtoWake = DeQ(&sleep_q);
     pcb[PIDtoWake].state=RUN;
     EnQ(PIDtoWake,&run_q);
   }
   ///////////////////////////////////////////////////////////////////////////////////////////
   // just return if CRP is Idle (0) or less (-1)
   if (CRP <= 0  ){
      return;
   }

   if(pcb[CRP].runtime == TIME_LIMIT){
    
      pcb[CRP].total_runtime=pcb[CRP].runtime + pcb[CRP].total_runtime;
      pcb[CRP].runtime=0;
      pcb[CRP].state = RUN;
      EnQ(CRP,&run_q);
      CRP = -1;
    
   }
   // return 0;
}


void ChkSleepQ(){
  int pid, size;
  size = sleep_q.size;
  while(size--){
  pid = DeQ(&sleep_q);

  if(pcb[pid].wake_time == sys_time){
    pcb[pid].state = RUN;
    EnQ(pid,&run_q);
  }else{
   EnQ(pid,&sleep_q);
  }//end else

  }//end while

}//end ChkSleepQ

void SleepISR(int seconds){
  outportb(0x20,0x60);
  wakeLength= sys_time+(100*seconds);
  pcb[CRP].wake_time=wakeLength;
  EnQ(CRP,&sleep_q);
  pcb[CRP].state=SLEEP;
  CRP=-1;
  return;
  
}

void SemWaitISR(int SemID){
  if(SemID < Q_SIZE){
    if(semaphore[SemID].count > 0){
      semaphore[SemID].count--;
    }else{
      EnQ(CRP,&(semaphore[SemID].wait_q));
      pcb[CRP].state = WAIT;
      CRP = -1;
    }//end else
  }//end if if(SemID < Q_SIZE)

} // end SemWaitISR


void SemPostISR(int SemID){

  int pid_DeQ;

  if(SemID < Q_SIZE){
    pid_DeQ = DeQ(&(semaphore[SemID].wait_q));

    if(pid_DeQ == -1){
      semaphore[SemID].count++;
    }else{
      pcb[pid_DeQ].state = RUN;
      EnQ(pid_DeQ,&run_q);
    }
  }  //end if(semaphoreID < Q_SIZE)

}

int SemGetISR(int count){
  int SemID;

  SemID = DeQ(&semaphore_q);

  if(SemID >= 0){
    MyBZero((char *) &semaphore[SemID], sizeof(semaphore_t));

    semaphore[SemID].count = count;
  } // end if(SemID >=0)

  return SemID;
}

void MsgSndISR(){
  
  msg_t *src, *dest;
  int msg;
  src = (msg_t *)pcb[CRP].TF_ptr->ebx;
  msg = src -> recipient;
  
  src->sender = CRP;
  src->time_stamp = sys_time;
  if ((mbox[msg].wait_q).size == 0){
    
    MsgEnQ(src, &mbox[msg].msg_q);
  }else{
    int tmp_pid = DeQ(&(mbox[msg].wait_q));
    EnQ(tmp_pid, &run_q);
    pcb[tmp_pid].state = RUN;
    dest = (msg_t *)pcb[tmp_pid].TF_ptr->ebx;
    memcpy((char*)dest,(char*)src, sizeof(msg_t));
  }
}

  void MsgRcvISR(){
  
  msg_t *temp;
  int pid;
  if(mbox[CRP].msg_q.size == 0){
    EnQ(CRP, &mbox[CRP].wait_q);
  pcb[CRP].state = WAIT;
  CRP=-1;
  }else{

    temp = MsgDeQ(&mbox[CRP].msg_q);
    *((msg_t *)pcb[CRP].TF_ptr->ebx) = *temp;
  }
}





void IRQ7ISR(){ //phase 4
  outportb(0x20,0x67); // Dismiss IRQ7
  if(print_semaphore >= 0){
    SemPostISR(print_semaphore);
  }


}

void IRQ3ISR(){
  int event;
  outportb(0x20,0x63);
  event = inportb(COM2_IOBASE+IIR);
  switch (event){//reading event from COM2_IOBASE+IIR
    case IIR_TXRDY:
      IRQ3TX();
      break;
    case IIR_RXRDY:
      IRQ3RX();
      break;  
  }  

  if(terminal.TX_extra==1) IRQ3TX();
}

void IRQ3TX(){
  char chr = '\0';

  if(terminal.echo_q.size !=0){
    chr = (char) DeQ(&terminal.echo_q); //set ch to the first element in exho_q 
  }else{
    if(terminal.TX_q.size != 0){
      chr = (char) DeQ(&terminal.TX_q);
      SemPostISR(terminal.TX_sem);
    }
  }

  if(chr == '\0'){
    terminal.TX_extra = 1;
  } else {
    outportb(COM2_IOBASE+DATA,chr); //sending char to COM2_IOBASE
    terminal.TX_extra = 0;
  }
}

void IRQ3RX() { // queue char read from port to RX and echo queues
      char chr;

      // use 127 to mask out msb (rest 7 bits in ASCII range)
      chr = inportb(COM2_IOBASE+DATA) & 0x7F;  // mask 0111 1111
      EnQ(chr , &terminal.RX_q);//enqueue ch to RX queue
      SemPostISR(terminal.RX_sem);//SemPostISR( RX semaphore of terminal interface )

      if(chr == '\r'){//if ch is '\r' {
         EnQ((int) '\r', &terminal.echo_q);//enqueue '\r' 
         EnQ((int) '\n', &terminal.echo_q);//then '\n' to echo queue of terminal interface
      } else {
         if(terminal.echo == 1){//if echo of terminal interface is 1 {
             EnQ((int) chr , &terminal.echo_q);//enqueue ch to echo queue of terminal interface
         }
      }
  
}


