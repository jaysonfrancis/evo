/**
 * @copyright (C) 2015 by Team LeftShark CSUS CPE159
 * @file   syscall.h
 * @author Brandon Ortiz (brandon@velwave.com)
 * @author Benjamin Smith (benjamin.smith.cs@gmail.com)
 * @date   March 1, 2015
 * @brief  
 *
 */

#ifndef _SYSCALL_H_
#define _SYSCALL_H_

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

extern int GetPid(void);      // no input, 1 return
extern void Sleep(int);   // 1 input, no return
extern void SemWait(int semaphoreID);
extern void SemPost(int semaphoreID);
extern int SemGet(int count);

#endif

/****************************************************************************
 *
 * End of file.
 *
 ****************************************************************************/
