// Derived from IRQ.C
// Derivation by Seang Chau

#include <dos.h>
#include "irq.h"

#ifdef __cplusplus
    // if it's a C++ program, ISRs take on ... argument
    #define __CPPARGS ...
#else
    // ortherwise nothing
    #define __CPPARGS
#endif

void interrupt far (*oldisr)(__CPPARGS);  //global to store old handler pointer

void sendEOI()
{
   //send non-specific to master AND slave PIC
   outportb(0x20, 0x20);
   outportb(0xA0, 0x20);
}

/*void interrupt ISR(__CPPARGS)
{
   sendEOI();
   // ????? clear the board's interrupt bit ?????
}
*/

unsigned char initirq(char IRQnumber,void interrupt (*ISR)())
{
   unsigned char intmask, oldmask;

   if (IRQnumber <=7) {
      oldmask = inportb(0x21);
      oldisr = getvect(IRQnumber + 8);
      setvect(IRQnumber + 8,ISR);
      intmask = oldmask & (~(1 << IRQnumber));
      outportb(0x21,intmask);
   } else {
      oldmask = inportb(0xA1);
      oldisr = getvect(IRQnumber + 0x70 - 8);
      setvect(IRQnumber - 8 + 0x70, ISR);
      intmask = oldmask & (~(1 << (IRQnumber - 8)));
      outportb(0xA1, intmask);
   }
   return(oldmask);
}

void restoreirq(char IRQnumber)
{
   unsigned char intmask;

   if (IRQnumber <=7 ){
      intmask = inportb(0x21);
      intmask |= (1 << IRQnumber);
      setvect(IRQnumber + 8,oldisr);
      outportb(0x21, intmask);
   } else {
      intmask = inportb(0xA1);
      intmask |= (1 << (IRQnumber - 8));
      setvect(IRQnumber - 8 + 0x70, oldisr);
      outportb(0xA1, intmask);
   }
}
