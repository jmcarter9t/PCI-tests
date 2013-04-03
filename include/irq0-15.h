#ifndef _IRQ0_15_H
#define _IRQ0_15_H
// IRQ0-15.H
// Allows installation of ISR into any interrupt request line

struct interupt_service_request_t  {
  int foo;
};

typedef struct interupt_service_request_t ISR;




// kills the current interrupt, no matter where it is
void sendEOI(void);

// returns the old PIC mask
// sets ISR to IRQnumber service routine
unsigned char initirq(int IRQnumber, void (*ISR)(void)  );
unsigned char initirq2(int IRQnumber);

// restores IRQnumber service routine
void restoreirq(char IRQnumber);
void restoreirq(char IRQnumber, unsigned char OldMask);


#endif
