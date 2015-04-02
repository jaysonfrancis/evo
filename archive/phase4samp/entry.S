/**
 * @copyright (C) 2015 by Team LeftShark CSUS CPE159
 * @file   entry.S
 * @author Brandon Ortiz (brandon@velwave.com)
 * @author Benjamin Smith (benjamin.smith.cs@gmail.com)
 * @date   February 16, 2015
 * @brief  
 *
 */

 /****************************************************************************
 *
 * Include any additional header files that are needed to compile this
 * header file here.
 *
 ****************************************************************************/

#include <spede/machine/asmacros.h>
#include "entry.h"

// set DS and ES to Kernel's data segment
#define SET_KSEGS movw $(KDATA), %ax; mov %ax, %ds; mov %ax, %es

.comm kstack, KSTACK_SIZE, 1 // define kernel stack space
.text

/****************************************************************************
 *
 * Place #define constants and macros that are required for this header
 * file or that need to be seen by more than one source file here.
 *
 ****************************************************************************/
// Dispatch() loads registers from trapframe to run
ENTRY(Dispatch)
   movl 4(%esp), %eax   // load stack pointer from eax
   movl %eax, %esp
   popl %gs
   popl %fs
   popl %es
   popl %ds
   popa                 // restore general registers
   add $4, %esp         // skip 4 bytes that stored intr_num
   iret

// push intr type number then jump to common handler
ENTRY(TimerEntry)
   pushl $TIMER_INTR
   jmp IntoKernel

// push intr type number then jump to common handler
ENTRY(GetPidEntry)
   pushl $GETPID_INTR
   jmp IntoKernel

// push intr type number then jump to common handler
ENTRY(SleepEntry)
   pushl $SLEEP_INTR
   jmp IntoKernel

// push intr type number then jump to common handler
ENTRY(SemWaitEntry)
   pushl $SEMWAIT_INTR
   jmp IntoKernel

// push intr type number then jump to common handler
ENTRY(SemPostEntry)
   pushl $SEMPOST_INTR
   jmp IntoKernel

// push intr type number then jump to common handler
ENTRY(SemGetEntry)
   pushl $SEMGET_INTR
   jmp IntoKernel

// push intr type number then jump to common handler
ENTRY(IRQ7Entry)
   pushl $IRQ7_INTR
   jmp IntoKernel


// Common IRQ entry, save context and call Kmain()
IntoKernel:
   pusha
   pushl %ds
   pushl %es
   pushl %fs
   pushl %gs
   movl %esp, %edx
   cld
   SET_KSEGS      // set DS and ES to kernel's data segments
   leal kstack + KSTACK_SIZE, %esp
   pushl %edx
   call CNAME(Kernel)
 /****************************************************************************
 *
 * Place function bodies here.
 *
 ****************************************************************************/

