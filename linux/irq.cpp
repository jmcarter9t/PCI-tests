#include "irq.h"

unsigned int baseadd;
unsigned int iiro_isr_iiroflag;


// kills the current interrupt, no matter where it is
void sendEOI(void)
{

}

// returns the old PIC mask
// sets ISR to IRQnumber service routine
unsigned char initirq(int IRQnumber, void (*ISR)(void)  )
{
  unsigned char retval;
  return retval;
}
unsigned char initirq2(int IRQnumber)
{
  unsigned char retval;
  return retval;
}

// restores IRQnumber service routine
void restoreirq(char IRQnumber)
{

}
void restoreirq(char IRQnumber, unsigned char OldMask)
{


}

/* temporary placeholder ...will fail*/
void interrupt iiro16_setiiroflag(void)
{
  iiro_isr_iiroflag = 1;
  /* OUTPORTB(baseadd+1,0x00); */
  /* sendEOI(); */
}


void interrupt setiiroflag(void)
{

}
