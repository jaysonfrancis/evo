/**
 * @copyright (C) 2015 by Team LeftShark CSUS CPE159
 * @file   proc.c
 * @author Brandon Ortiz (brandon@velwave.com)
 * @author Benjamin Smith (benjamin.smith.cs@gmail.com)
 * @date   February 16, 2015
 * @brief  Processes are here
 *
 */

/****************************************************************************
 *
 * Place #include files required to compile this source file here.
 *
 ****************************************************************************/

#include "spede.h"   // for IO_DELAY() needed here below
#include "extern.h"  // for current_run_pid needed here below
#include "proc.h"    // for Idle, SimpleProc, DispatchProc)
#include "tool.h"
#include "syscall.h"

/****************************************************************************
 *
 * Define global variables that will be used by both this source file
 * and by other source files here.
 *
 ****************************************************************************/

int print_semaphore;

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

/** @brief Idle function stalls cpu for ~1 second, intended for use in the idle process
 *
 *  @return void
 *
 */
void Idle(void) 
{
   while(1)
   {
     cons_printf("  0 ");

     delay_ms(1000);
   }
}

/** @brief User Process
 *
 *  @return void
 *
 */
void UserProc(void) 
{
   int pid;

	while(1)
   {
      pid = GetPid();

	   //print its pid (CRP) on PC
	   cons_printf(" %2x ", pid);

      Sleep(4 - pid%4); //wait time as specified in lab manual
	}
}

/** @brief PrintDriver
 *
 *  @return void
 *
 */
void PrintDriver(void) 
{
   int i;
   int code;
   int pid;
   const char str[] = "Hello, my team is called Left Shark!\n\0";
   const char * p;

   print_semaphore = SemGet(0);

   outportb((LPT1_BASE + LPT_CONTROL), PC_SLCTIN);
   code = inportb(LPT1_BASE + LPT_STATUS);
   
   for (i = 0; i < 50; i++)
   {
      IO_DELAY();
   }

   outportb((LPT1_BASE + LPT_CONTROL), (PC_INIT | PC_SLCTIN | PC_IRQEN));

   Sleep(1);
   
   while(1)
   {
      pid = GetPid();

	   cons_printf(" p%x ", pid);

      Sleep(1);

      if (print_it)
      {
         p = str;

         while (*p != '\0')
         {
            outportb((LPT1_BASE + LPT_DATA), *p);
            code = inportb(LPT1_BASE + LPT_CONTROL);

            outportb((LPT1_BASE + LPT_CONTROL), (code | PC_STROBE));

            for (i = 0; i < 20; i++)
            {
               IO_DELAY();
            }

            outportb((LPT1_BASE + LPT_CONTROL), code);

            SemWait(print_semaphore);

            p++;
         }

         print_it = 0;
      }
   }
}

/****************************************************************************
 *
 * End of file.
 *
 ****************************************************************************/
