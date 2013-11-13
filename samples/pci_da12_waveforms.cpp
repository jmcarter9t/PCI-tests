/***************************************************************************
 *  This sample will generate 3 different wave forms, sine, triangle and   *
 *  saw tooth.  The user has the choice of base address, DAC number and the*
 *  number of points per cycle.                                            *
 ***************************************************************************/
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

#define  PI    3.1415927

unsigned       base;              /* card base address          */
unsigned       counts;            /* number of points per cycle */
unsigned       dacnum;            /* DAC used for output        */
unsigned       progstruct[20000]; /* buffer to hold points      */

/***************************************************************************
 *  FUNCTION: setparams() - local routine                                  *
 *  PURPOSE: Prompts the user for DAC number, base address and the number *
 *            of points per cycle.                                         *
 ***************************************************************************/
void setparms(void)
{
  char msg[6];
  CLRSCR();
  CPRINTF("Enter the DAC number you wish to output to (0 through 15):\n");
  GETS(msg);
  sscanf(msg,"%u",&dacnum);
  dacnum %= 16;
  CPRINTF("Enter the number of points you wish to calculate per cycle,\n");
  CPRINTF("(20000 maximum, program will use modulus if needed) :");
  GETS(msg);
  sscanf(msg,"%u",&counts);
  counts%=20001;
}  /* end setparams */

/***************************************************************************
 *  FUNCTION: sendtoport() - local routine                                 *
 *  PURPOSE: Writes the point buffer to the DAC until a key is pressed.   *
 ***************************************************************************/
void sendtoport(void)
{
int           i,temp;
long          j;
unsigned      counts_out;

/* each point is written to the DAC */
do {
     for( i = 0;i < counts;i++) { 
       OUTPORT(base+(dacnum*2), progstruct[i]);
       usleep(100);
     }
     printf("");
   }
while (!kbhit());
 nodelay(stdscr,0);
OUTPORT(base+(dacnum*2),0);    /* set DAC to 0 output */
} /* end sendtoport */

/***************************************************************************
 *  FUNCTION: sinecurve() - local routine                                  *
 *  PURPOSE: Calculate the points for creating a sine wave.           .   *
 ***************************************************************************/
void sinecurve(void)
{
int             i;
double          rads,sine;

if (counts == 0) return;           /* no point -- no curve */

CLRSCR();
CPRINTF("Calculating sine wave points.....");

rads = (double) 2 * PI / (counts - 1);   /* rad per count */

for(i = 0;i < counts;i++)
   {
   sine = (sin(rads * i) + 1.0) * 2047; /* 2047 is 12-bit scaling factor */
   progstruct[i] = (unsigned) sine;
   }
CLRSCR();
CPRINTF("Generating sine wave, press any key to stop....");
sendtoport();
} /* end sinecurve */

/***************************************************************************
 *  FUNCTION: trianglecurve() - local routine                              *
 *  PURPOSE: Calculate the points for creating a triangle wave.           *
 ***************************************************************************/
void trianglecurve(void)
{
int             i;
double          slope,temp;

if (counts == 0) return;            /* no counts -- no curve */

CLRSCR();
CPRINTF("Calculating triangle wave points.....");

slope = 4095.0 / counts * 2.0;
					/* wave form slope       */
for(i=0;i < counts/2;i++)
   {
   temp = slope * i;
   progstruct[i] = (int)temp;
   temp = 4095.0 - temp;
   progstruct[i+counts/2+1] = (int)temp;
   }
CLRSCR();
CPRINTF("Generating triangle wave, press any key to stop....");
sendtoport();
}  /* end trianglecurve */

/***************************************************************************
 *  FUNCTION: sawcurve() - local routine                                   *
 *  PURPOSE: Calculate the points for creating a saw tooth wave.          *
 ***************************************************************************/
void sawcurve(void)
{
int             i;
double          slope,temp;

if (counts == 0) return;

CLRSCR();
CPRINTF("Calculating saw tooth wave points.....");

slope = 4095.0 / counts;       /* saw tooth slope */

for(i = 0;i < counts;i++)
   {
   temp = slope * i;
   progstruct[i] = (int) temp;
   progstruct[i] %= 4095;
   }
CLRSCR();
CPRINTF("Generating saw tooth wave, press any key to stop....");
sendtoport();
} /* end sawcurve */

/***************************************************************************
 *  FUNCTION: menulist() - local routine                                   *
 *  PURPOSE: Display the menu choise on the screen.                       *
 ***************************************************************************/
void menulist(void)
{
  CLRSCR();
  CPRINTF("\n\n\n");
  CPRINTF("Your menu selections are:\n");
  CPRINTF("1. Input Board Data  (do this first)\n");
  CPRINTF("2. Sine curve\n");
  CPRINTF("3. Triangle curve\n");
  CPRINTF("4. Saw curve\n");
  CPRINTF("5. End program, return to DOS\n");
  CPRINTF("Input Choice: ");
}  /* end menulist */


/***************************************************************************
 * FUNCTION: AskForBaseAddress                                             *
 * PURPOSE: Prompt user to enter base address for their I/O card.          *
 **************************************************************************/
unsigned AskForBaseAddress(unsigned int OldOne)
{
	char msg[7];
	int NewOne = 0, Success = 0, Dummy;
	int AddrInputPosX, AddrInputPosY;

	CPRINTF("\nPlease enter the Base Address for your card (in hex)");
	CPRINTF("or press ENTER for %X.\n>", OldOne);
	AddrInputPosX = WHEREX(); AddrInputPosY = WHEREY();
	do {
          GOTOXY(AddrInputPosX, AddrInputPosY);
          // CLREOL();
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
   "                         C SAMPLE #2: SAMPLE2.C                          \n"
   "\n"
   "  This sample will generate 3 different wave forms, sine, triangle and   \n"
   "  saw tooth.  The user has the choice of base address, DAC number and the\n"
   "  number of points per cycle.                                            \n"
   "\n"
   "\n" );

   CPRINTF("Please press any key to set base address");
   GETCH();
   base = AskForBaseAddress(0x350);
   // CLRSCR();
   } /* end of intro */

/***************************************************************************
 *  FUNCTION: main() - local routine                                       *
 ***************************************************************************/
int main(int argc, char *argv[])
{
  int   menuchoice;
  int    Ch;
  int    done;
  char msg[7];

  intro();

  /* Set all 16 buffers to 0 */
  for (Ch = 0; Ch < 4; Ch++)
    OUTPORT(base + (Ch*2), 0x800);

  INPORTB(base+10);       // set automatic update mode
  INPORTB(base+15);       // release zero latch

  CLRSCR();
  done = 0;
  do
    {
      memset(progstruct,0,sizeof(int));  /* clear buffer               */
      menulist();                        /* display the menu           */
      GETS(msg);
      sscanf(msg, "%d", &menuchoice );
      // menuchoice=GETCH();                /* fetch the menu choice      */
      // CPRINTF("%d\n",menuchoice );
      switch(menuchoice)                 /* execute the menu selection */
        {
        case 1: 
          setparms();           /* fetch the system params    */
          break;
        case 2: 
          sinecurve();          /* generate a sine wave       */
          break;
        case 3: 
          trianglecurve();      /* generate a triangle wave   */
          break;
        case 4: 
          sawcurve();           /* generate a saw tooth wave  */
          break;
        case 5:                       /* exit sample program.       */
          done = 1;
        }; /* end switch */
      menuchoice = 0;
    } while( !done );

  /* Set all 16 buffers to 0 */
  for (Ch = 0; Ch < 4; Ch++)
    OUTPORT(base + (Ch*2), 0x800);

  PUTS("%s sample2 complete.", argv[0] );
  ENDWIN();
} /* end main */
