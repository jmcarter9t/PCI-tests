/****************************************************************************
 *  This sample will prompt the user for a voltage between 0 and 10 volts,  *
 *  then calculate the actual voltage based on the resolution of the DAC and*
 *  output the voltage to the desired DAC channel.                          *
 ****************************************************************************/
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

/* Global card base address */
unsigned base, EBase;

/****************************************************************************
 *                                                                          *
 *  FUNCTION: Cal                                                           *
 *                                                                          *
 *   PURPOSE: Calibrate                                                     *
 *                                                                          *
 *   INPUT: Value and channel                                               *
 *                                                                          *
 *   OUTPUT: Calibrated value                                               *
 *                                                                          *
 ****************************************************************************/
unsigned Cal(unsigned InValue, unsigned Ch)
{
   char a, b;
   unsigned OutValue;
   unsigned  offset = 0xf0;
   int temp = (Ch * 2) + INPORTB(EBase + offset + Ch)*32;
   a = INPORTB(EBase + ((Ch * 2) + (INPORTB(EBase + 240 + Ch) * 32)) + 1);
   b = INPORTB(EBase + ((Ch * 2) + (INPORTB(EBase + 240 + Ch) * 32)));
   OutValue = ((4095.0 - a - b) / 4095.0) * InValue + b;
   return OutValue;
}

/****************************************************************************
 *  FUNCTION: write_DAC() - local routine                                   *
 *  PURPOSE: Prompts the user for DAC number and voltage, then calculates  *
 *            the actual output voltage based on resolution, displays it    *
 *            and writes the value to the DAC.                              *
 ****************************************************************************/
void write_DAC(void)
{
float           volt_value_entered,volt_value_expected;
unsigned        counts_entered,dac_number;

/* prompt for the DAC number and desire voltage. */
CPRINTF("Enter the DAC number (0 through 15 only)\n: ");
scanf("%u",&dac_number);
CPRINTF("\n\nEnter a voltage between 0.000v to 9.997v\n: ");
scanf("%f",&volt_value_entered);

/* convert entered voltage to associated number of DAC counts. Scale
   factor for 12 bit resolution of DA12-16 is 0.002442. */
volt_value_entered /= 0.002442;

/* compute the digital output needed for this value and the actual voltage
   that will be expected, and display the voltage. */

counts_entered=(int) volt_value_entered;
volt_value_expected = (float) counts_entered * 0.002442;
CPRINTF("\nDue to the 12-bit resolution of the DAC, you should expect\n");
CPRINTF("to see a voltage of %4.3f.",volt_value_expected);

/* write to the DAC */
OUTPORT(base + (2 * dac_number), Cal(counts_entered, dac_number)); /* write low byte      */

} /* end write_DAC */


/***************************************************************************
 * FUNCTION: AskForBaseAddress                                             *
 * PURPOSE: Prompt user to enter base address for their I/O card.          *
 **************************************************************************/
unsigned AskForBaseAddress(unsigned int OldOne, char *TypeString)
{
	char msg[7];
	int NewOne = 0, Success = 0, Dummy;
	int AddrInputPosX, AddrInputPosY;

	CPRINTF("\nPlease enter the %sBase Address for your card (in hex)\n", TypeString);
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


/***************************************************************************
 * FUNCTION: intro                                                         *
 * PURPOSE: Present user with introductory screen. Get base address.       *
 **************************************************************************/
void intro(void)
   {
   CLRSCR();
   CPRINTF(
   "                        C SAMPLE #1: SAMPLE1.C                          \n\n"
   "  This sample will prompt the user for a voltage between 0 and 10 volts,  \n"
   "  then calculate the actual voltage based on the resolution of the DAC and\n"
   "  output the voltage to the desired DAC channel.                        \n\n"
   "  The following setup of the board is expected:                         \n\n"
   "        þ All DAC voltage ranges should be set to 0v-10v unipolar.        \n\n");

   PUTS("\n");
   PUTS("Press any key to continue ... ");
   GETCH();
   CLRSCR();
   base = AskForBaseAddress(0x350, "");
   CLRSCR();
   EBase = AskForBaseAddress(0x350, "Calibration ");
   } /* end of intro */


/****************************************************************************
 *  FUNCTION: main() - local routine                                        *
 ****************************************************************************/
int main(int argc, char *argv[])
    {
    int key_entered, Ch;

    intro();

    /* Set all 16 buffers to 0 */
    for (Ch = 0; Ch < 4; Ch++)
	   OUTPORT(base + (Ch * 2), Cal(0x000, Ch));

    inb(base+0xA);    // set automatic update mode
    inb(base+0XF);    // release zero latch

    do
	{
	CLRSCR();
	write_DAC();
	CPRINTF("\n\nWould you like to output another value (Y or N)?\n");
	key_entered = GETCH();
	} while((key_entered != 'N') && (key_entered != 'n'));

    /* Set all 16 buffers to 0 */
    for (Ch = 0; Ch < 4; Ch++)
	   OUTPORT(base + (Ch * 2), Cal(0x000, Ch));

    CLRSCR();
    PUTS("DA12-16 Sample1 complete.");
    } /* end main */




