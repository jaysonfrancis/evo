/**
 * @copyright (C) 2015 by Team LeftShark CSUS CPE159
 * @file   spede.h
 * @author Brandon Ortiz (brandon@velwave.com)
 * @author Benjamin Smith (benjamin.smith.cs@gmail.com)
 * @date   February 16, 2015
 * @brief  SPEDE library headers, ~spede2/Target-i386/Common/include/spede
 *
 */

#ifndef _SPEDE_H_
#define _SPEDE_H_

/****************************************************************************
 *
 * Include any additional header files that are needed to compile this
 * header file here.
 *
 ****************************************************************************/

#include <spede/flames.h>           // IO_DELAY(), breakpoint(), cons_putchar(), cons_getchar(), cons_kbhit(),
#include <spede/sys/cdefs.h>        // __BEGIN_DECLS, __END_DECLS, etc.
#include <spede/stdio.h>            // printf(), sprintf(), vprintf(), sscanf(), etc.
#include <spede/assert.h>           // assert()

#include <spede/machine/io.h>       // inportb(), outportb(), inportw(), etc.
#include <spede/machine/parallel.h>
#include <spede/machine/proc_reg.h> // get_idt_base(), get_cs(), set_cr3(), etc
#include <spede/machine/seg.h>      // struct i386_gate, fill_gate(), etc.
#include <spede/machine/asmacros.h> // ASM(), CNAME(), ENTRY(), etc.

#include <spede/machine/pic.h>   // IRQ_VECTOR(), IRQ_TIMER, IRQ_COM1, ICU0_IOBASE, SPECIFIC_EOI(), etc.
#include <spede/stdlib.h>        // TRUE, FALSE, NULL, etc.
#include <spede/string.h>        // memset(), strcpy(), strtok(), etc.
#include <spede/time.h>          // CLK_TCK, time_t, clock(), time(), etc.

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

#endif

/****************************************************************************
 *
 * End of file.
 *
 ****************************************************************************/
