/**
 * @copyright (C) 2015 by Team LeftShark CSUS CPE159
 * @file   syscall.c
 * @author Brandon Ortiz (brandon@velwave.com)
 * @author Benjamin Smith (benjamin.smith.cs@gmail.com)
 * @date   March 1, 2015
 * @brief  
 *
 */

/****************************************************************************
 *
 * Place #include files required to compile this source file here.
 *
 ****************************************************************************/

#include "syscall.h"

/****************************************************************************
 *
 * Define global variables that will be used by both this source file
 * and by other source files here.
 *
 ****************************************************************************/

/****************************************************************************
 *
 * Place #define constants and macros that will be used only by this
 * source file here.
 *
 ****************************************************************************/

/****************************************************************************
 *
 * Place typedefs, structs, unions, and enums that will be used only
 * by this source file here.
 *
 ****************************************************************************/

/****************************************************************************
 *
 * Place static variables that will be used only by this file here.
 *
 ****************************************************************************/

/****************************************************************************
 *
 * Place function prototypes that will be used only by this file here.
 * This section need only include "forward referenced" functions.
 *
 ****************************************************************************/

/****************************************************************************
 *
 * Place function bodies here.
 *
 ****************************************************************************/

/** @brief GetPid
 *
 *  @return void
 *
 */
int GetPid(void) 
{
   int pid;

   asm("int $48; movl %%ebx, %0" // CPU inst
       : "=g"  (pid)             // 1 output from asm() 
       :                         // no input into asm()
       : "%ebx");                // push/pop before/after asm()
    
   return pid;
}

/** @brief Sleep
 *
 *  @param int sec
 *  @return void
 *
 */
void Sleep(int sec) 
{
   asm("movl %0, %%eax; int $49"
       :                   // no output
       : "g" (sec)         // input
       : "eax");
}

/** @brief SemWait
 *
 *  @return void
 *
 */
void SemWait(int semaphoreID) 
{
   asm("movl %0, %%eax; int $50"    // CPU inst
       :                            // no output from asm() 
       : "g" (semaphoreID)          // 1 input into asm()
       : "eax");                    // push/pop before/after asm()
}

/** @brief SemPost
 *
 *  @return void
 *
 */
void SemPost(int semaphoreID) 
{
   asm("movl %0, %%eax; int $51"    // CPU inst
       :                            // no output from asm() 
       : "g" (semaphoreID)          // 1 input into asm()
       : "eax");                    // push/pop before/after asm()
}

/** @brief GetPid
 *
 *  @return void
 *
 */
int SemGet(int count) 
{
   int semaphore_id;

   asm("movl %1, %%eax; int $52; movl %%ebx, %0" // CPU inst
       : "=g"  (semaphore_id)             // 1 output from asm() 
       : "g" (count)                        // no input into asm()
       : "ebx", "eax");                // push/pop before/after asm()
    
   return semaphore_id;
}

/****************************************************************************
 *
 * End of file.
 *
 ****************************************************************************/
