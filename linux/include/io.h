#ifndef IO_H
#define IO_H

#include "common_objects.h"
#include <ncurses.h>


#define OUTPORT(address,value) outb(value,address)
#define OUTPORTB(address,value) outb(value,address)
#define INPORT inb_p
#define INPORTB inb

#define interrupt
#define far 


#define DELAY(x)  usleep(x*1000)
#ifndef CLK_TCK
#define CLK_TCK      CLOCKS_PER_SEC
#endif

#define INTERRUPT 

int kbhit(void);


#endif
