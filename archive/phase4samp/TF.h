/**
 * @copyright (C) 2015 by Team LeftShark CSUS CPE159
 * @file   TF.h
 * @author Brandon Ortiz (brandon@velwave.com)
 * @author Benjamin Smith (benjamin.smith.cs@gmail.com)
 * @date   February 16, 2015
 * @brief  Defines the TrapFrame type, TF_t
 *
 */

#ifndef _TF_H_
#define _TF_H_

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

#ifndef ASSEMBLER
 
// This structure corresponds to the state of user registers 
// as saved upon kernel trap/interrupt entry. 
 
typedef unsigned short seg_type_t; // 16-bit segment value 
 
typedef struct { 
// Saved segment registers 
	seg_type_t	gs;          // unsigned short, 2 bytes 
	seg_type_t	_notgs;      // filler, 2 bytes 
	seg_type_t	fs; 
	seg_type_t	_notfs; 
	seg_type_t	es; 
	seg_type_t	_notes; 
	seg_type_t	ds; 
	seg_type_t	_notds; 
 
// PUSHA register state frame 
	unsigned int	edi; 
	unsigned int	esi; 
	unsigned int	ebp; 
	unsigned int	esp;  // Push: before PUSHA, Pop: skipped 
	unsigned int	ebx; 
	unsigned int	edx; 
	unsigned int	ecx; 
	unsigned int	eax; 
 
// to indicate the type of interrupt that has happened
	unsigned int	intr_num; 
 
// Processor state frame 
	unsigned int	eip; 
	unsigned int	cs; 
	unsigned int	eflags; 
} TF_t; 
 
#endif ASSEMBLER

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

#endif _TF_H_

/****************************************************************************
 *
 * End of file.
 *
 ****************************************************************************/
