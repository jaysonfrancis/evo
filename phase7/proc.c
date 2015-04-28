// proc.c, 159
// processes are here
// Team Evo 

#include "spede.h"   // for IO_DELAY() needed here below
#include "syscall.h"  // Added in Phase 2
#include "tool.h"
#include "extern.h"  // for current_run_pid needed here below
#include "proc.h"     // for Idle, SimpleProc, DispatchProc

void Idle() {
	int i; // Added for seconds
	for (;;) { // Infinity loop
		cons_printf("0 ");   // Print 0 ON host PC
		//Sleep(1);
		for (i = 0; i < 1666000; i++)
			IO_DELAY();    //busy-loop delay for about 1 sec
	}    // End infinite loop
}

void UserProc() {
	int thePID;

	// Added in the loop. ? thePID = GetPid();
	//x = 4 - (CRP%4); // Calculate proper seconds. Formula taken from 2.html

	for (;;) {
		thePID = GetPid();
		cons_printf("%d ", thePID); // Print its PID. GetPid() isn't working here?
		Sleep(4 - thePID % 4);

	} // End infinite loop
}

// PrintDriver Added in Phase 4

void PrintDriver() {
	int i, code, thePID;
	msg_t my_msg;
	// char str[] = "Hello, my Team is called Evo!\n\0";
	char *p;

	print_semaphore = SemGet(-1); // should it be -1? depends on IRQISR()

// make sure printer's powered up, cable connected, the following
// statements (until the Sleep call) will reset the printer and the
// first IRQ 7 will occur

	outportb((LPT1_BASE + LPT_CONTROL), PC_SLCTIN); // Printer Control, SeLeCT INterrupt
	code = inportb(LPT1_BASE + LPT_STATUS);
	for (i = 0; i < 50; i++)
		IO_DELAY();             // needs delay
	outportb(LPT1_BASE + LPT_CONTROL, PC_INIT | PC_SLCTIN | PC_IRQEN); // IRQ ENable
	Sleep(1);                                   // printer needs time to reset

	while (1) {
		thePID = GetPid();
		cons_printf("%d ", thePID);

		MsgRcv(&my_msg);
		p = my_msg.data;
		while (*p) {
			outportb(LPT1_BASE + LPT_DATA, *p);
			code = inportb(LPT1_BASE + LPT_CONTROL);
			outportb(LPT1_BASE + LPT_CONTROL, code | PC_STROBE);
			for (i = 0; i < 20; i++)
				IO_DELAY();
			outportb(LPT1_BASE + LPT_CONTROL, code);
			SemWait(print_semaphore);
			p++;
		}
	}
}

void Init() {
	int key;
	char str[] = "Hello, Evo team for 159\n";
	msg_t msg;
	msg.recipient = 2;
	MyStrcpy(msg.data, str);

	while (1) {
		cons_printf(" %d ", GetPid());
		Sleep(1);

		if (cons_kbhit()) {
			key = cons_getchar();
			switch (key) {
			case 'b':
				breakpoint();
				break;
			case 'q':
				exit(0);
			case 'p':
				MsgSnd(&msg);
				break;
			default:
				cons_printf("Error - No command\n");
				break;
			}
		}
	}
}

void Shell() { //phase 6
	int BAUD_RATE, divisor;
	msg_t msg;
	char login[101], password[101];
	int STDIN = 4, STDOUT = 5;
	int final;
// Clear 3 Queues
	MyBZero((char *) &terminal.TX_q, sizeof(q_t));
	MyBZero((char *) &terminal.RX_q, sizeof(q_t));
	MyBZero((char *) &terminal.echo_q, sizeof(q_t));

	terminal.TX_sem = SemGet(Q_SIZE); //Get a sem to set TX_sem, count Q_SIZE
	terminal.RX_sem = SemGet(0); // Get sem to set RX_sem, count is 0
	terminal.echo = 1; // set echo to 1
	terminal.TX_extra = 1; // set TX_extra to 1

	// COM1-8_IOBASE: 0x3f8 0x2f8 0x3e8 0x2e8 0x2f0 0x3e0 0x2e0 0x260
	// transmit speed 9600 bauds, clear IER, start TXRDY and RXRDY
	// Data communication acronyms:
	//    IIR Intr Indicator Reg
	//    IER Intr Enable Reg
	//    ETXRDY Enable Xmit Ready
	//    ERXRDY Enable Recv Ready
	//    MSR Modem Status Reg
	//    MCR Modem Control Reg
	//    LSR Line Status Reg
	//    CFCR Char Format Control Reg
	//    LSR_TSRE Line Status Reg, Xmit+Shift Regs Empty

	// set baud rate 9600
	BAUD_RATE = 9600;              // Mr. Baud invented this
	divisor = 115200 / BAUD_RATE;  // time period of each baud
	outportb(COM2_IOBASE + CFCR, CFCR_DLAB);          // CFCR_DLAB 0x80
	outportb(COM2_IOBASE + BAUDLO, LOBYTE(divisor));
	outportb(COM2_IOBASE + BAUDHI, HIBYTE(divisor));
	// set CFCR: 7-E-1 (7 data bits, even parity, 1 stop bit)
	outportb(COM2_IOBASE + CFCR, CFCR_PEVEN | CFCR_PENAB | CFCR_7BITS);
	outportb(COM2_IOBASE + IER, 0);
	// raise DTR, RTS of the serial port to start read/write
	outportb(COM2_IOBASE + MCR, MCR_DTR | MCR_RTS | MCR_IENABLE);
	IO_DELAY();
	outportb(COM2_IOBASE + IER, IER_ERXRDY | IER_ETXRDY); // enable TX, RX events
	IO_DELAY();

	// infinitie loop

	while (1) {
		while (1) { // Loop A
			MyStrCpy(msg.data, "Valid Commands are: whoami, bye\n\0");

			msg.recipient = STDOUT;
			MsgSnd(&msg);
			MsgRcv(&msg);

			MyStrCpy(msg.data, "login: \0");
			msg.recipient = STDOUT;
			MsgSnd(&msg);
			MsgRcv(&msg);

			msg.recipient = STDIN;
			MsgSnd(&msg);
			MsgRcv(&msg);
			MyStrCpy(login, msg.data);

			MyStrCpy(msg.data, "password: \0");
			msg.recipient = STDOUT;
			MsgSnd(&msg);
			MsgRcv(&msg);

			msg.recipient = STDIN;
			MsgSnd(&msg);
			MsgRcv(&msg);
			MyStrCpy(password, msg.data);

			final = MyStrcmp(login, password);
			if (final) {
				break;
			} else if (final == 0) {
				MyStrCpy(msg.data, "Invalid login!\n\0");
				msg.recipient = STDOUT;
				MsgSnd(&msg);
				MsgRcv(&msg);
			}

		} // Loop A

		while (1) { // Loop B
			MyStrCpy(msg.data, "Enter command: \0"); // prompt for entering command string
			msg.recipient = STDOUT; // get command string entered


			MsgSnd(&msg);
         	MsgRcv(&msg);  
         	//get command string entered
        	msg.recipient=STDIN;
        	MsgSnd(&msg);
        	MsgRcv(&msg);


			if (MyStrlen(msg.data) == 0) {
				continue;
			}  //if command string is empty {
			if (MyStrcmp(msg.data, "bye\0")) {
				break;
			}
			if (MysStrcmp(msg.data, "whoami\0")) {
				MyStrCpy(msg.data, login);
				MsgSnd(&msg);
				MsgRcv(&msg);
				MyStrCpy(msg.data, "\n\0");
				msg.recipient = STDOUT;
				MsgSnd(&msg);
				MsgRcv(&msg);
				continue;
			} else {
				MsgStrCpy(msg.data, "Command not found!\n\0");
				msg.recipient = STDOUT;
				MsgSnd(&msg);
				MsgRcv(&msg);
			}
		} //repeat loop B
	}

}

void STDIN(void) {
	char *p, ch;
	msg_t msg;

	while (1) {
		MsgRcv(&msg);
		p = msg.data;

		while (1) {
			SemWait(terminal.RX_sem);
			ch = DeQ(&terminal.RX_q);
			if (ch == '\r') {
				break;
			}
			p++;
			p = ch;
		}
		p = '\0';
		msg.recipient = msg.sender;
		msg.sender = CRP;
		MsgSnd(&msg);
	}
}

void STDOUT(void) {
	msg_t msg;
	char *p;

	while (1) { //infinite loop:
		MsgRcv(&msg);
		p = msg.data;

		while (*p != '\0') {
			SemWait(terminal.TX_sem);
			EnQ( (int)p, &terminal.TX_q);
			if (*p == '\n') {
				SemWait(terminal.TX_sem);
				EnQ((int) '\r', &terminal.TX_q);
			}
			p++;
		}
		msg.recipient = msg.sender;
		MsgSnd(&msg);
	}
}



void ShellDirStr(attr_t *p, char *str) {
   // p points to attr_t and then obj name (p+1)
      char *obj = (char *)(p + 1);

   // make str from the attr_t that p points to
      sprintf(str, " - - - -  size =%6d     %s\n", p->size, obj);
      if ( A_ISDIR(p->mode) ) str[1] = 'd';         // mode is directory
      if ( QBIT_ON(p->mode, A_ROTH) ) str[3] = 'r'; // mode is readable
      if ( QBIT_ON(p->mode, A_WOTH) ) str[5] = 'w'; // mode is writable
      if ( QBIT_ON(p->mode, A_XOTH) ) str[7] = 'x'; // mode is executable
   }


oid ShellDir(char *cmd, int STDOUT, int FileMgr) {
      char obj[101], str[101];
      msg_t msg;

   // if cmd is "dir\0" (or "333\0") assume root: "dir /\0"
   // else, there should be an obj after 1st 4 letters "dir "
      if(MyStrcmp(cmd, "dir\0") == 1 || MyStrcmp(cmd, "333\0") == 1) {
         obj[0] = '/';
         obj[1] = '\0';                           // null-terminate the obj[]
      } else {
         cmd += 4;         // skip 1st 4 letters "dir " and get the rest (obj)
         MyStrCpy(obj, cmd); // make sure cmd is null-terminated from Shell()
      }

   //*************************************************************************
   // write code:
   MyStrCpy(msg,obj);// apply standard "check object" protocol
   msg.code=80;//    prep msg: put correct code and obj into msg
   
   //    send msg to FileMgr, receive reply, chk result code
   msg.recipient=FileMgr;
   MsgSnd(&msg);
   MsgRcv(&msg);
   // if code is not GOOD
   if(msg.code != 1){
      MyStrCpy(msg.data,"Error Obj not good \n\0");//    prompt error msg via STDOUT
      msg.recipient=STDOUT;
      MsgSnd(&msg);
      MsgRcv(&msg);   //    receive reply
      return;//    return;        // cannot continue
   }//*************************************************************************

   //*************************************************************************
   // otherwise, code is good, returned msg has an "attr_t" type,
   // check if user directed us to a file, then "dir" for that file;
   // write code:
   // p = (attr_t *)msg.data;
   //
   // if( ! A_ISDIR(p->mode) ) {
   //    ShellDirStr(p, str);        // str will be built and returned
   //    prep msg and send to STDOUT
   //    receive reply
   //    return;
   // }
   //*************************************************************************

   //*************************************************************************
   // if continued here, it's a directory
   // request to open it, then issue read in loop
   // write code:
   // apply standard "open object" protocol
   // prep msg: put code and obj in msg
   // send msg to FileMgr, receive msg back (should be OK)
   //
   // loop
   //    apply standard "read object" protocol
   //    prep msg: put code in msg and send to FileMgr
   //    receive reply
   //    if code came back is not GOOD, break loop
   //    (if continued, code was good)
   //    do the same thing with ShellDirStr() like above
   //    then show str via STDOUT
   // }
   //*************************************************************************
   //*************************************************************************
   // write code:
   // (abbreviated below since same as what done above)
   // apply standard "close object" protocol with FileMgr
   // if response is not GOOD, display error msg via STDOUT...
   //*************************************************************************
}
 
void ShellTyp(char *cmd, int STDOUT, int FileMgr) {
   char obj[101], str[101]; // get away without obj
   attr_t *p;
   msg_t msg;

   cmd+=4;
   StrCpy(msg.data, cmd) // ?
   msg.recipient = 6;
   msg.code = CHK_OBJ;
   MsgSnd(&msg);
   MsgRcv(&msg);

   if ( ( msg.code != GOOD ) || A_ISDIR(p->mode) ) 
   {
 	PromptUser("Usage: typ [path]<filename>\n\0"); // ??
    return; } else {
      while (1)
      {
         msg.recipient = 6;
         msg.code = READ_OBJ;
         MsgSnd(&msg);
         MsgRcv(&msg);

         if ( msg.code == GOOD ) { PromptUser(msg.data); }
         else { break; }
      }
   }  
   msg.recipient = 6;
   msg.code = CLOSE_OBJ;
   MsgSnd(&msg);
   MsgRcv(&msg);
   if ( msg.code != GOOD ) { PromptUser("FD object err"); }

 }



