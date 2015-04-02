/**
 * @copyright (C) 2015 by Team LeftShark CSUS CPE159
 * @file   type.h
 * @author Brandon Ortiz (brandon@velwave.com)
 * @author Benjamin Smith (benjamin.smith.cs@gmail.com)
 * @date   February 16, 2015
 * @brief  
 *
 */

#ifndef _TYPE_H_
#define _TYPE_H_

/****************************************************************************
 *
 * Include any additional header files that are needed to compile this
 * header file here.
 *
 ****************************************************************************/

#include "TF.h"

/****************************************************************************
 *
 * Place #define constants and macros that are required for this header
 * file or that need to be seen by more than one source file here.
 *
 ****************************************************************************/

#define TIME_LIMIT 300       //!max timer count to run
#define MAX_PROC 20          //!max number of processes
#define Q_SIZE 20            //!queue depth
#define STACK_SIZE 4096      //!process stack size in bytes

/****************************************************************************
 *
 * Place typedefs, structs, unions, and enums that need to be seen by
 * multiple source files here.
 *
 ****************************************************************************/

// this is the same as constants defined: KMODE=0, UMODE=1
typedef enum 
{
   KMODE,      //!< Kernel Mode
   UMODE,      //!< User Mode

   NUM_OF_MODE
} mode_t;

typedef enum 
{
   NONE,
   RUNNING,       
   RUN, 
   SLEEP, 
   WAIT, 
   ZOMBIE,

   NUM_OF_STATE
} state_t;

// PCB describes proc image
typedef struct 
{             
   TF_t *TF_ptr;             //!< Points to TF in stack
   int runtime;              //!< Run time since dispatched
   int total_runtime;        //!< Total run time since created
   int wake_time;            //!< Time to wake up
   mode_t mode;              //!< Process privilege mode
   state_t state;            //!< State of process   
} pcb_t;

typedef struct 
{            
   int q[Q_SIZE];    //!< Indices into q[] array to place or get element
   int head;         //!< Index of head
   int tail;         //!< Index of tail
   int size;         //!< Number of items in queue
} q_t;

typedef struct 
{
   q_t wait_q;
   int count;   
} semaphore_t;

//! Void-returning, void argument function pointer type
typedef void (* func_ptr_t)(void); 

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

#endif

/****************************************************************************
 *
 * End of file.
 *
 ****************************************************************************/
