/**
 * @copyright (C) 2015 by Team LeftShark CSUS CPE159
 * @file   tool.c
 * @author Brandon Ortiz (brandon@velwave.com)
 * @author Benjamin Smith (benjamin.smith.cs@gmail.com)
 * @date   February 16, 2015
 * @brief  
 *
 */

/****************************************************************************
 *
 * Place #include files required to compile this source file here.
 *
 ****************************************************************************/

#include "spede.h"
#include "type.h"
#include "extern.h"
#include "tool.h"

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

//VMware has their PIT interval all strange...
#ifdef VM
   #define INSTRUCTIONS_PER_MS 100000
#else
   #define INSTRUCTIONS_PER_MS 1666000
#endif
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


/** @brief Zero 
 *
 *  @param *p 
 *  @param size
 *  @return void
 *
 */
void MyBZero(char * p, const int size) 
{
   int i;

   for (i = 0; i < size; i++)
   {
      *p++ = 0;
   }
}

/** @brief Enqueue item
 *
 *  @param pid process ID
 *  @param *p pointer to queue
 *  @return void
 *
 */
void EnQ(const int pid, q_t * const p) 
{
   asrt(p->size < Q_SIZE , "EnQ passed pointer too large: %x", p->size)
   if (p->size < Q_SIZE)
   {
      p->size++;

      p->q[p->tail] = pid;

      if (p->tail >= Q_SIZE - 1)
      {
         p->tail = 0;
      }
      else
      {
        p->tail++;
      }
   }
   else
   {
    return -1;
   }
}

/** @brief Dequeue item
 *
 *  @param *p pointer to queue 
 *  @return -1 if q is empty
 *
 */
int DeQ(q_t * const p) 
{ 
   int item;

   asrt(p->size >= 0 , "DeQ passed pointer too small: %x", p->size)

   if (p->size > 0)
   {
      p->size--;

      item = p->q[p->head];
      p->q[p->head] = 5555; //for easier queue debug

      if (p->head >= Q_SIZE)
      {
         p->head = 0;
      }
      else
      {
        p->head++;
      }
   }
   else
   {
      item = -1;
   }

   return item;
}

/** @brief millisecond busy wait
 *
 *  @param ms_delay function will busy-wait CPU for ms_delay milliseconds when called
 *  @return void
 *
 */
void delay_ms(const unsigned int delay)
{
   unsigned int i;
   unsigned int delay_count;

   delay_count = delay * INSTRUCTIONS_PER_MS;

   for(i = 0; i < delay_count; i++) 
   {
      // I/O port 0x80 is reserved and will never be used.  If you
      // access this port, you create a delay of three cycles at 8.33MHz,
      // plus the I/O access, which takes two clock cycles (total of 5).
      // An unmapped I/O port goes out to the ISA bus, which runs with
      // a 8.33MHz clock. Especially true in PCI systems.
      // Two of these in a row give a 1.2 microsecond delay.
      //      Attributed to James van Artsdalen.
      ASM ("inb $0x80,%%al" : : : "eax");
   }
}

/****************************************************************************
 *
 * End of file.
 *
 ****************************************************************************/
