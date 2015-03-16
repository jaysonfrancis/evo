/**
 * @copyright (C) 2015 by Team LeftShark CSUS CPE159
 * @file   entry.h
 * @author Brandon Ortiz (brandon@velwave.com)
 * @author Benjamin Smith (benjamin.smith.cs@gmail.com)
 * @date   February 16, 2015
 * @brief  
 *
 */

#ifndef _ENTRY_H_
#define _ENTRY_H_

/****************************************************************************
 *
 * Include any additional header files that are needed to compile this
 * header file here.
 *
 ****************************************************************************/

#include <spede/machine/pic.h>

/****************************************************************************
 *
 * Place #define constants and macros that are required for this header
 * file or that need to be seen by more than one source file here.
 *
 ****************************************************************************/

#define TIMER_INTR      32
#define IRQ7_INTR       39
#define GETPID_INTR     48
#define SLEEP_INTR      49
#define SEMWAIT_INTR    50
#define SEMPOST_INTR    51
#define SEMGET_INTR  52

#define KCODE 0x08    // kernel's code segment
#define KDATA 0x10    // kernel's data segment
#define KSTACK_SIZE 8192  // kernel's stack size, in chars

/****************************************************************************
 *
 * Place typedefs, structs, unions, and enums that need to be seen by
 * multiple source files here.
 *
 ****************************************************************************/

/****************************************************************************
 *
 * Place global variables that are used by multiple source files here.
 *
 ****************************************************************************/

/****************************************************************************
 *
 * Place function prototypes that need to be seen by multiple source
 * files here.
 *
 ****************************************************************************/

// ISR Entries
#ifndef ASSEMBLER

	__BEGIN_DECLS

	#include "type.h" // for trapframe_t below

	extern void TimerEntry(void);     // code defined in entry.S
	extern void GetPidEntry(void);     // code defined in entry.S
	extern void SleepEntry(void);     // code defined in entry.S
	extern void SemWaitEntry(void);     // code defined in entry.S
	extern void SemPostEntry(void);     // code defined in entry.S
	extern void SemGetEntry(void);     // code defined in entry.S
	extern void Dispatch(TF_t *); // code defined in entry.S
	extern void IRQ7Entry(void);     // code defined in entry.S

	__END_DECLS

#endif

/****************************************************************************
 *
 * End of file.
 *
 ****************************************************************************/
#endif
