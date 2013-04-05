 
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
#include "iiro16.h"
#include "pciutil.h"


int
main(int argc, char *argv[])
{
  int base_address = 0xd000;
  int relayoffset;
  unsigned char RelayOffset = 8;
  int index;
  CLRSCR();
  IOPermissions();

  GLOBALS.result = read_configuration_word(GLOBALS.bn,GLOBALS.df,GLOBALS.regnum = 0x14,&GLOBALS.basea); //enable IRQ generation
  GLOBALS.basea &= 0xFFFE;
  OUTPORTB(GLOBALS.basea+0x69,0x09);


  GLOBALS.result = read_configuration_word(GLOBALS.bn, GLOBALS.df, GLOBALS.regnum = 0x18, &GLOBALS.basea);
  GLOBALS.basea  &=  0xFFFE;
  GLOBALS.result += read_configuration_word(GLOBALS.bn, GLOBALS.df, GLOBALS.regnum = 0x3C, &GLOBALS.irqnum);
  GLOBALS.irqnum  &=  0xF;

  GLOBALS.result = read_configuration_word(GLOBALS.bn,GLOBALS.df,GLOBALS.regnum = 0x14,&GLOBALS.basea); //enable IRQ generation
  GLOBALS.basea &= 0xFFFE;
  OUTPORTB(GLOBALS.basea+0x69,0x09);

  GLOBALS.result = read_configuration_word(GLOBALS.bn,GLOBALS.df,GLOBALS.regnum = 0x18,&GLOBALS.basea);
  GLOBALS.basea  &=  0xFFFE;
  GLOBALS.result += read_configuration_word(GLOBALS.bn,GLOBALS.df,GLOBALS.regnum = 0x3C,&GLOBALS.irqnum);
  GLOBALS.irqnum  &=  0xF;
  GLOBALS.result = 0;
  GLOBALS.basea = 0xd100;
  GLOBALS.irqnum = 5;

  if ( GLOBALS.result == 0 ){
    //test if card found
    TEXT_COLOR(LIGHTGRAY);
    // CPRINTF("%40s\n\r",GLOBALS.name);
    TEXT_COLOR(GLOBALS.result?RED:GREEN);
    CPRINTF("BASE:%04X  IRQ:%hu%40s\n\r",GLOBALS.basea, GLOBALS.irqnum, GLOBALS.result?"FAILED":"PASSED");

    unsigned char RelayOffset = 8;
    do
      {
        RelayOffset -= 4;

        TEXT_COLOR(LIGHTGRAY);
        CPRINTF("   %s Port\n\r", RelayOffset ? "HI" : "LO");
        CPRINTF("Install an IIRO-16 spider cable with an IIRO-16 wrap plug on the %s port.\n\r", RelayOffset ? "HI" : "LO");
        CPRINTF("Apply +5V to red wrap plug lead (Relay commons).\n\r");
        CPRINTF("Apply GND to black wrap plug lead (Isolated input lows).\n\r");
        CPRINTF("Press any key to start test.\n\r");
        GETCH();

        GLOBALS.result = iiro16_relay_write_test(GLOBALS.basea,GLOBALS.DebugFlag, RelayOffset);
        TEXT_COLOR(GLOBALS.result?RED:GREEN);
        CPRINTF("      Relay Write Test: %47s\n\r",GLOBALS.result?"FAILED":"PASSED");

        GLOBALS.result = iiro16_relay_step_test(GLOBALS.basea,GLOBALS.DebugFlag, RelayOffset);
        TEXT_COLOR(GLOBALS.result?RED:GREEN);
        CPRINTF("      Relay Step Test: %48s\n\r",GLOBALS.result?"FAILED":"PASSED");

        GLOBALS.result = iiro16_soft_filter_test(GLOBALS.basea,GLOBALS.DebugFlag, RelayOffset);
        TEXT_COLOR(GLOBALS.result?RED:GREEN);
        CPRINTF("      Software Filter Test: %43s\n\r",GLOBALS.result?"FAILED":"PASSED");

        GLOBALS.result = iiro16_interrupt_test(GLOBALS.basea,GLOBALS.irqnum, 0 , RelayOffset);
        TEXT_COLOR(GLOBALS.result?RED:GREEN);
        CPRINTF("      Interrupt Test: %49s\n\r",GLOBALS.result?"FAILED":"PASSED");

      } while (RelayOffset);
  }
  ENDTEST();
}




