/*-------------------------------------------------------------------
 * DA12-16 C language sample 3. Program illustrates access to
 * simultaneous and automatic modes.
 *------------------------------------------------------------------*/
#include <stdio.h>
#include <stdlib.h>
#include <sys/io.h>
#include <time.h>
#include <unistd.h>
#include <termios.h>
#include <sys/time.h>
#include <sys/select.h>
#include <math.h>

#include "common_objects.h"
#include "display.h"
#include "io.h"
#include "iiro8.h"
#include "pciutil.h"


/* Provide global access to card's base address */
unsigned base;

/*-------------------------------------------------------------------
 * OutputValue
 * Program the output value of a specified DA channel
 *------------------------------------------------------------------*/
void OutputValue(char CH, unsigned VALUE)
{
  OUTPORT(base + (CH * 2), VALUE);
} /* end OutputValue */

/*-------------------------------------------------------------------
 * FUNCTION: AskForBaseAddress
 * PURPOSE: Prompt user to enter base address for their I/O card.
 -------------------------------------------------------------------*/
unsigned AskForBaseAddress(unsigned int OldOne)
{
  char msg[7];
  int NewOne = 0, Success = 0, Dummy;
  int AddrInputPosX, AddrInputPosY;

  PUTS("\nPlease enter the Base Address for your card (in hex)");
  CPRINTF("or press ENTER for %X.\n>", OldOne);
  AddrInputPosX = WHEREX(); AddrInputPosY = WHEREY();
  do {
    GOTOXY(AddrInputPosX, AddrInputPosY);
    CLREOL();
    msg[0] = 5; msg[1] = 0;
    GETS(msg);
    sscanf(msg + 2, "%x", &NewOne);
    IOPermission(NewOne);
          
    Success = 1;
    Dummy = NewOne;
    if (msg[1] == 0) {
      GOTOXY(AddrInputPosX, AddrInputPosY);
      CPRINTF("%X", OldOne);
      Success = 1;
      Dummy = OldOne;
    }
  } while(!Success);
  return (Dummy);
} /* end of AskForBaseAddress */


/*-------------------------------------------------------------------------
 * FUNCTION: intro
 * PURPOSE: Present user with introductory screen. Get base address.
 -------------------------------------------------------------------------*/
void intro(void)
{
  CLRSCR();
  PUTS("\n");
  PUTS("                         C Sample #3 : SAMPLE3.C");
  PUTS("\n");
  PUTS("This sample program demonstrates both automatic and simultaneous");
  PUTS("update modes of the DA12-16.");
  PUTS("\n");
  PUTS("Note: This sample assumes a 5 Volt range on all DACs.\n");
  PUTS("\n");
  PUTS("\n");
  PUTS("Press any key to set base address ... ");
  GETCH();
  base = AskForBaseAddress(0x300);
  CLRSCR();
} /* end intro */


/*-------------------------------------------------------------------
 *                                 Main
 *------------------------------------------------------------------*/
int main(int argc, char *argv[] )

{
  unsigned Channel = 0, Value = 0;
  char key = 0;
  char msg[20];

  intro();

  PUTS("Should I perform a hardware-reset clear?");
  PUTS("Answer Y if the card was just powered-up,");
  PUTS("or N if you've already done this since the last reset.");

  do {
    key = toupper(GETCH());
  } while ((key != 'Y') && (key != 'N'));

  PUTS("");
  if (key == 'Y')
    {
      for (Channel = 0; Channel < 4; Channel++)
        OutputValue(Channel, 0x800);
      /*
        Note: 0x000 if Unipolar Mode (per channel)
        Be aware that the OutputValue routine does not necessarily
        update the outPUTS.  In this particular case, it does not.
        The card powers up in simultaneous Update mode, therefore,
        the output command is not reflected in the voltages until
        an update command is issued  --  the next line:
      */

      INPORTB(base + 10);
      /*
        This updates the outPUTS, and exits simultaneous mode.  We
        could have remained in simultaneous mode and still updated
        the outPUTS by using base + 8 instead of +10.
      */

      INPORTB(base + 15);
      /* Release zero latch */

      PUTS("All of the outPUTS have been updated with Bipolar zero (0x800)");
      PUTS("Assuming power-on condition, nothing should have changed.");
      PUTS("(Incidentally, any channels that are in the Unipolar range");
      PUTS("should have simultaneously changed to +2V outputs.)");
      PUTS("\n");
    } /* if 'Y' */

  PUTS("Press any key to output 2V on all channels, non-simultaneously");
  GETCH();
  PUTS("\n");
  INPORTB(base + 2); // remove from simultaneous, offset
  // 10 would also work.
  for (Channel = 0; Channel < 4; Channel++)
    OutputValue(Channel,0xC00); // these each take effect immediately.

  INPORTB(base + 0); // place in simultaneous mode

  for (Channel = 0; Channel < 4; Channel++)
    OutputValue(Channel,0xFFF); // these don't do a thing to
  // the outPUTS yet.
  PUTS("All channels have been written to with 0xFFF, but the outPUTS don't");
  PUTS("change.  Press any key to initiate simultaneous update of all channels.");
  PUTS("Outputs will all change at the same time to 4.9998V DC.");
  GETCH();
  PUTS("\n");

  INPORTB(base + 8); // still in simultaneous mode, updated outPUTS.
  PUTS("All channels should have changed to full-scale.\n");
  PUTS("\n");
  PUTS("Press any key to continue ... ");
  GETCH();
  PUTS("\n");

  // note that we don't issue a Simultaneous Update Mode read command -- we're
  // still in simultaneous mode from above.
  for (Channel = 0; Channel <= 4; Channel += 2)
    OutputValue(Channel, 0x000);  // these don't do a thing to
  // the outPUTS yet.

  PUTS("Channels 0, 2 have been written to with 0x000, but");
  PUTS("the outPUTS haven't changed.");
  PUTS("Press any key to change channels 0  and 2 to -5V, simultaneously.");
  GETCH();
  PUTS("\n");

  INPORTB(base + 10); // update and remove from simultaneous mode.

  PUTS("The channels should have changed now.");
  PUTS("\n");
  PUTS("Press any key to continue with program.");
  PUTS("\n");

  do {
    CLRSCR();
    PUTS("The program will now allow you to type in any count value desired");
    PUTS("for output on any channel desired");
    PUTS("Enter ChannelNumber Space HexCounts  Ex: 15 A00 [enter]");
    do {
      GETS(msg);
      sscanf(msg,"%i %x", &Channel, &Value );
      if (Channel >= 16)
        puts("Channel must be less than 16.");
    } while (Channel >= 16);
      
    OutputValue(Channel, Value); // this happens immediately, not
    // in simultaneous mode
    PUTS("Press [ENTER] to Exit Program, or any other key to continue.");
    key = (char)GETCH();
    CPRINTF("")
  } while( key != '\n' );

  PUTS("%s sample2 complete.", argv[0] );
  ENDWIN();
}
