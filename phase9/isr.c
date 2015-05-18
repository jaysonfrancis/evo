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

      pcb[pid].main_table = sys_main_table; // Phase 9  Tip #3
      
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


// Added phase 8

void ForkISR(){
   int new_pid, page_num, exec_addr, index[5], *p, main_table, code_table, stack_table, code_page, stack_page, i, x;

   // A. Allocate a new PID and five RAM pages
   new_pid = DeQ(&none_q);
  
      // A.1 if no PID available, consprintf, set ecx of CRP TF to -1, end this ISR
   if ( new_pid == -1 ) { 
        cons_printf("ForkISR(): No More PID available!\n");
        pcb[CRP].TF_ptr->ecx = -1;
        return;     
        }


      // A.2 (otherwise) allocate new_pid ??????
      else {
        pcb[CRP].TF_ptr->ecx = new_pid;

      }
        // ????????????????????????????? //
        x=0;

      // A.3 loop to scan page[], if see one page available, set page to new_pid, index this page, if got five, break loop
      for (i=0; i<MAX_PROC; i++){
        if(page[i].owner == -1){
          page[i].owner=new_pid;
          index[x] = i;
          x++;
          if (x==5){ break; }
        }
      }


      // A.4 if didn't get five indicies, cons print not enough mem, recycle/return new_pid, loop to return pages ust obtained, set ecx of CRP TF to -1, end isr
      if (x!=5){
        cons_printf("ForkISR(): not enough memory available!\n");
        EnQ(new_pid, &none_q);
        for(i=0; i<MAX_PROC; i++){
          if(page[i].owner == new_pid){ page[i].owner=-1;}
        }
      }


      // B. with five pages obtained, set these int, main_table, code_table, stack_table, code_page, stack_page, to address of obtained pages
      main_table = page[index[0]].addr;
      code_table = page[index[1]].addr;
      stack_table = page[index[2]].addr;
      code_page = page[index[3]].addr;
      stack_page = page[index[4]].addr;

      // C. Copy entries 0 to 15 from ss_main_table to main_table
      MyMemcpy((char*)main_table, (char*)sys_main_table, 16);

      // D. Set entries 512 and 767 of main_table to addresses of code_Table and stack_table, add two flags into entries
      p=(int*)(main_table + (512*4));
      *p = code_table + 0x003;  // flags

      p=(int*)(stack_table + (767*4));
      *p = stack_page + 0x003;  // flags

      // E. set entry 0 of code_Table to addr of code_page (plus flags)
      p=(int*)(code_table + 0);
      *p = code_page + 0x003;

      // F. set entry 1023 of stack_Table to addr of stack_page (plus flags)
      p=(int*)(stack_table + (1023*4));
      *p = stack_pge + 0x003; // flagss

      // G. Copy executable to code_page (like prev phase)
      MyMemcpy((char*)code_page, (char*)pcb[CRP].TF_ptr->ebx, 4096);

      // H. Set things in its PCB
      pcb[new_pid].main_table = main_table;
      pcb[new_pid].runtime = 0;
      pcb[new_pid].total_runtime = 0;
      pcb[new_pid].state = RUN;
      pcb[new_pid].ppid = CRP;
      pcb[new_pid].mode = UMODE;

      // I. build it's trapframe
      pcb[new_pid].TF_ptr = (TF_t*) (stack_page+4032);
      pcb[new_pid].TF_ptr->eip = (unsigned int)(0x80000000); // 2G+128
      pcb[new_pid].TF_ptr->eflags = EF_DEFAULT_VALUE | EF_INTR;
      pcb[new_pid].TF_ptr->cs = get_cs();
      pcb[new_pid].TF_ptr->ds = get_ds();
      pcb[new_pid].TF_ptr->es = get_es();
      pcb[new_pid].TF_ptr->fs = get_fs();
      pcb[new_pid].TF_ptr->gs = get_gs();
      pcb[new_pid].TF_ptr->(TF_t*) (0xbfffffc0); // 3G-64

      // J. Clear mailbox of this new process
      MyBZero((char*)&mbox[new_pid], sizeof(mbox_t));

      // K. Enqueue the pid of this new process to run queue
      EnQ(new_pid, &run_q);

    }
   
void WaitISR(){
  int j, i, child_exit_num, *parent_exit_num_ptr;
  int zproc;
  //looking for zombie process
  for(i = 0; i < MAX_PROC; i++){
    if(pcb[i].ppid==CRP && pcb[i].state==ZOMBIE) zproc = i;
  }//end for

  if (zproc!= MAX_PROC){
    pcb[CRP].TF_ptr->eax = zproc;

    child_exit_num = pcb[zproc].TF_ptr->ebx;
    parent_exit_num_ptr = (int*) pcb[CRP].TF_ptr->ebx;
    *parent_exit_num_ptr = child_exit_num;

    //putting CRP into run_q
    EnQ(CRP, &run_q);
    pcb[CRP].state = RUN;
    CRP = -1;

    //killing zombie process
    EnQ(zproc, &none_q);
    pcb[zproc].total_runtime += pcb[zproc].runtime;
    pcb[zproc].runtime = 0;
    pcb[zproc].state = NONE;

    //reclaiming memory
     //reclaim memory
   for(j = 0; j < MAX_PAGE; j++)
   {
      if(page[j].owner == zproc){
         page[j].owner = -1;
         MyBzero((char*)page[j].addr, 0x1000);
      }
   }

   zproc = -1;

  }else{
    pcb[CRP].state = WAIT_CHILD;
    CRP = -1;
  }//end else
}

void ExitISR(){
  int j, ppid, child_exit_num, *parent_exit_num_ptr;

  ppid = pcb[CRP].ppid;


  if(pcb[ppid].state == WAIT_CHILD){
    pcb[ppid].TF_ptr->eax = CRP;
    child_exit_num = pcb[CRP].TF_ptr->ebx;
    parent_exit_num_ptr = (int*) pcb[ppid].TF_ptr->ecx;
    *parent_exit_num_ptr = child_exit_num;

    EnQ(ppid, &run_q);
    pcb[ppid].state = RUN;

    EnQ(CRP, &none_q);
    pcb[CRP].total_runtime += pcb[CRP].runtime;
    pcb[CRP].runtime = 0;
    pcb[CRP].state = NONE;

    //reclaiming memory
     //reclaim memory
   for(j = 0; j < MAX_PAGE; j++)
   {
      if(page[j].owner == pid){
         page[j].owner = -1;
         MyBzero((char*)page[j].addr, 0x1000);
      }
   }


  }else{

    pcb[CRP].state = ZOMBIE;
  }
  CRP = -1;
}


