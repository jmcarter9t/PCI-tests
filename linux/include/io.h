#ifndef IO_H
#define IO_H

#define OUTPORT outb
#define OUTPORTB outb
#define INPORT inb_p
#define INPORTB inb

#define STRCMP   strcmp
#define GETCH    getchar


#define DELAY  usleep
#ifndef CLK_TCK
#define CLK_TCK      CLOCKS_PER_SEC
#endif

#define INTERRUPT 

#define READ_CONFIGURATION_WORD(w,x,y,z) 1

#endif
