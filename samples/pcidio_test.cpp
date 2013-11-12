#include <stdio.h>
#include <stdlib.h>
#include <sys/io.h>
#include <time.h>
#include <unistd.h>
#include <termios.h>
#include <sys/time.h>
#include <sys/select.h>

#include "common_objects.h"
#include "display.h"
#include "io.h"
#include "iiro8.h"
#include "pciutil.h"


int
main(int argc, char *argv[])
{
  // int base_address = 0xd100;
  int relayoffset;
  unsigned char RelayOffset = 8;
  int index;
  CLRSCR();
  IOPermissions();  

  PCIDIO(char *name, int numadd, int D48Flag, int COSFlag, int COUNTFlag, int HDFlag);

  // if (!GLOBALS.result){
  //   //test if card found
  //   TEXT_COLOR(LIGHTGRAY);
  //   TEXT_COLOR(GLOBALS.result?RED:GREEN);
  //   CPRINTF("BASE:%04X  IRQ:%hu%40s\n\r",GLOBALS.basea, GLOBALS.irqnum, GLOBALS.result?"FAILED":"PASSED");
  //   GLOBALS.result = interrupt_test(GLOBALS.basea,GLOBALS.irqnum,GLOBALS.DebugFlag);
  //   TEXT_COLOR(GLOBALS.result?RED:GREEN);
  //   CPRINTF("      Interrupt Test: %49s\n\r",GLOBALS.result?"FAILED":"PASSED");
  // }

  ENDTEST();
}

// GLOBALS.basea = 0xd100;
// GLOBALS.result = read_configuration_word(GLOBALS.bn,GLOBALS.df,GLOBALS.regnum = 0x14,&GLOBALS.basea); //enable IRQ generation
// GLOBALS.basea &= 0xFFFE;
// OUTPORTB(GLOBALS.basea+0x69,0x09);
// GLOBALS.result = read_configuration_word(GLOBALS.bn, GLOBALS.df, GLOBALS.regnum = 0x18, &GLOBALS.basea);
// GLOBALS.basea  &=  0xFFFE;
// GLOBALS.result += read_configuration_word(GLOBALS.bn, GLOBALS.df, GLOBALS.regnum = 0x3C, &GLOBALS.irqnum);
// GLOBALS.irqnum  &=  0xF;
// GLOBALS.result = read_configuration_word(GLOBALS.bn,GLOBALS.df,GLOBALS.regnum = 0x14,&GLOBALS.basea); //enable IRQ generation
// GLOBALS.basea &= 0xFFFE;
// OUTPORTB(GLOBALS.basea+0x69,0x09);
// GLOBALS.result = read_configuration_word(GLOBALS.bn,GLOBALS.df,GLOBALS.regnum = 0x18,&GLOBALS.basea);
// GLOBALS.basea  &=  0xFFFE;
// GLOBALS.result += read_configuration_word(GLOBALS.bn,GLOBALS.df,GLOBALS.regnum = 0x3C,&GLOBALS.irqnum);
// GLOBALS.irqnum  &=  0xF;
// GLOBALS.result = 0;
// GLOBALS.basea = 0xd100; /* Change this and the irq number */
// GLOBALS.irqnum = 5;
// TEXT_COLOR(RED);
// CPRINTF("Using baseaddress %x, and IRQ %d\n\r",GLOBALS.basea, GLOBALS.irqnum );




