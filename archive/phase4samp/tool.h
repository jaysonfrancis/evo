/**
 * @copyright (C) 2015 by Team LeftShark CSUS CPE159
 * @file   tool.h
 * @author Brandon Ortiz (brandon@velwave.com)
 * @author Benjamin Smith (benjamin.smith.cs@gmail.com)
 * @date   February 16, 2015
 * @brief  
 *
 */

#ifndef _TOOL_H
#define _TOOL_H

/****************************************************************************
 *
 * Include any additional header files that are needed to compile this
 * header file here.
 *
 ****************************************************************************/

#include "type.h" // q_t needs be defined in code below

/****************************************************************************
 *
 * Place #define constants and macros that are required for this header
 * file or that need to be seen by more than one source file here.
 *
 ****************************************************************************/

/** @brief assertion which stops program on failure
 *
 *  @param char asrt inteded to be used for result of boolean statements 
 *         assertion will fire and terminate program if asrt is 0
 *  @param char* pointer to string to print upon assertion failure
 *  @return void
 *
 */
#define asrt(cond, print_args_str, print_args)      \
  if (!(cond))                        \
  {                                   \
    printf("ASSERTION FAILURE: ");       \
    printf(print_args_str, print_args);       \
    cons_printf("\n ASSERTION FAILURE: ");       \
    cons_printf(print_args_str, print_args);  \
                                      \
    breakpoint();                     \
  }                                   \


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

extern void MyBZero(char * p, const int size);
extern void EnQ(const int pid, q_t * const p);
extern int DeQ(q_t * const p);
extern void delay_ms(const unsigned int delay);

#endif

/****************************************************************************
 *
 * End of file.
 *
 ****************************************************************************/
