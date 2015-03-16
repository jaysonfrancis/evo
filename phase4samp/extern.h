/**
 * @copyright (C) 2015 by Team LeftShark CSUS CPE159
 * @file   extern.h
 * @author Brandon Ortiz (brandon@velwave.com)
 * @author Benjamin Smith (benjamin.smith.cs@gmail.com)
 * @date   February 16, 2015
 * @brief  
 *
 */

#ifndef _EXTERN_H_
#define _EXTERN_H_

/****************************************************************************
 *
 * Include any additional header files that are needed to compile this
 * header file here.
 *
 ****************************************************************************/

#include "type.h"            // q_t, pcb_t, MAX_PROC, STACK_SIZE

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

extern int CRP;             
extern q_t run_q;
extern q_t none_q;    
extern pcb_t pcb[MAX_PROC];
extern char stack[MAX_PROC][STACK_SIZE];

// Phase 2
extern volatile int sys_time;
extern q_t sleep_q;

//Phase 3
extern semaphore_t semaphore[Q_SIZE];
extern q_t semaphore_q;    
extern int product_semaphore;
extern int product;

//Phase 4
extern int print_it;

/****************************************************************************
 *
 * Place function prototypes that need to be seen by multiple source
 * files here.
 *
 ****************************************************************************/

#endif

/****************************************************************************
 *
 * End of file.
 *
 ****************************************************************************/
