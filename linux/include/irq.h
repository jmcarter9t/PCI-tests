#ifndef IRQ_H
#define IRQ_H

#include "io.h"
// IRQ0-15.H
// Allows installation of ISR into any interrupt request line

struct interupt_service_request_t  {
  int foo;
};

typedef struct interupt_service_request_t ISR;

extern unsigned int baseadd;
extern unsigned int iiro16_isr_iiroflag;


// kills the current interrupt, no matter where it is
void sendEOI(void);

// returns the old PIC mask
// sets ISR to IRQnumber service routine
unsigned char initirq(int IRQnumber, void (*ISR)(void)  );
unsigned char initirq2(int IRQnumber);

// restores IRQnumber service routine
void restoreirq(char IRQnumber);
void restoreirq(char IRQnumber, unsigned char OldMask);

void interrupt iiro16_setiiroflag(void);
void interrupt setiiroflag(void);


#endif
