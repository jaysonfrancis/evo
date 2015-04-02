/**
 * @copyright (C) 2015 by Team LeftShark CSUS CPE159
 * @file   isr.h
 * @author Brandon Ortiz (brandon@velwave.com)
 * @author Benjamin Smith (benjamin.smith.cs@gmail.com)
 * @date   February 16, 2015
 * @brief  
 *
 */

#ifndef _ISR_H_
#define _ISR_H_

/****************************************************************************
 *
 * Include any additional header files that are needed to compile this
 * header file here.
 *
 ****************************************************************************/

/****************************************************************************
 *
 * Place #define constants and macros that are required for this header
 * file or that need to be seen by more than one source file here.
 *
 ****************************************************************************/

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

extern void CreateISR(const int pid);
extern void TerminateISR(void);
extern void TimerISR(void);
extern void GetPidISR(void);
extern void SleepISR(int s);
extern void SemWaitISR(int semaphoreID);
extern void SemPostISR(int semaphoreID);
extern int SemGetISR(int count);
extern void IRQ7ISR(void);

#endif

/****************************************************************************
 *
 * End of file.
 *
 ****************************************************************************/
