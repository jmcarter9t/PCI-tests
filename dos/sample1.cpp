#include <stdio.h>
#include <dos.h>
#include <conio.h>
#include "irq0-15.h"
/*************************************************************
	LAST MODIFIED: 8/28/98
*************************************************************/

typedef unsigned int uint;
int byte = 0xff;
unsigned IRQ;
int flag=0;
unsigned ADDRESS;

void CtrMode(uint addr, char cntr, char mode)
{
  int ctrl;
  ctrl = (cntr << 6) | 0x30 | (mode << 1);
  outportb(addr+3,ctrl);
}

void CtrLoad(uint addr ,int c,int val)
{
  outportb(addr+c,val & 0x00FF);
  outportb(addr+c,(val>>8) & 0x00FF);
}

unsigned AskForBaseAddress(unsigned int OldOne)
{
	char msg[7];
	int NewOne = 0, Success = 0, Dummy;
	int AddrInputPosX, AddrInputPosY;

	puts("\nPlease enter the Base Address for your card (in hex)");
	printf("or press ENTER for %X.\n>", OldOne);
	AddrInputPosX = wherex(); AddrInputPosY = wherey();
	do {
		gotoxy(AddrInputPosX, AddrInputPosY);
		clreol();
		msg[0] = 5; msg[1] = 0;
		cgets(msg);
		sscanf(msg + 2, "%x", &NewOne);
		Success = 1;
		Dummy = NewOne;
		if (msg[1] == 0) {
		  gotoxy(AddrInputPosX, AddrInputPosY);
		  printf("%X", OldOne);
		  Success = 1;
		  Dummy = OldOne;
		}
	} while(!Success);
	return (Dummy);
} /* end of AskForBaseAddress */

unsigned AskForBaseIRQ(unsigned int OldOne)
{
	char msg[7];
	int NewOne = 0, Success = 0, Dummy;
	int AddrInputPosX, AddrInputPosY;

	puts("Please enter the IRQ for your card (in hex)");
	printf("or press ENTER for %X.\n>", OldOne);
	AddrInputPosX = wherex(); AddrInputPosY = wherey();
	do {
		gotoxy(AddrInputPosX, AddrInputPosY);
		clreol();
		msg[0] = 6; msg[1] = 0;
		cgets(msg);
		sscanf(msg + 2, "%x", &NewOne);
		Success = 1;
		Dummy = NewOne;
		if (msg[1] == 0) {
			gotoxy(AddrInputPosX, AddrInputPosY);
			printf("%X", OldOne);
			Success = 1;
			Dummy = OldOne;
		}
	} while(!Success);
	return (Dummy);
} /* end of AskForBaseIRQ */

void interrupt setflag()
{
  flag=1;  //read character
  byte = inportb(ADDRESS+4);
  outportb(0xA0,0x20);  //disable interrupt for upper irq
  outportb(0x20,0x20);  //disable interrupt for lower irq
}//end setflag

/*
******************************************************************************
FUNCTION set_counter()
Parameters:  None
Returns:     None

Purpose: This function performs the setup operations on the 82C54 counter/
	 timer chip.
Calls:       None
Uses:        ADDRESS            Constant defined at top of program.
******************************************************************************
*/
void set_counter(void)
{
  inportb(ADDRESS+7);
  CtrMode(ADDRESS,2,0);  //set counter 2 to mode 0
  outportb(ADDRESS+7,0);  //enable counters
  inportb(ADDRESS+7);    //disable couter functions
  outportb(ADDRESS+6,0);   //enable buzzer
  outportb(ADDRESS+0x0c,0); //select low clock rate
  inportb(ADDRESS+0x0d);  //disable opto reset
  inportb(ADDRESS+0x0e);  //disable opto reset
  inportb(ADDRESS+0x0f);  //disable program outputs

  CtrMode(ADDRESS, 0, 3); // set counter 0 to mode 3
  CtrLoad(ADDRESS, 0, 0xFFFF); // load counter 0 with full scale

  CtrMode(ADDRESS, 1, 2); // set counter 1 to mode 2
  CtrLoad(ADDRESS, 1, 0x0010); // load counter 1 with 10 hex

  CtrMode(ADDRESS, 2, 1); // set counter 2 to mode 1
  //CtrLoad(ADDRESS,2,0xFFFF);
  //Note: Not loading counter 2 creates an infinite reset duration
  //Set the reset duration by loading a value in counter 2
  //Change counter 2 to mode 2 to set duration
} /* End set_counter() */



/*
******************************************************************************
FUNCTION main()
Parameters: None
Returns:    None

Purpose: This function controls program flow, making calls to the other
	 functions, and prompting for user input.
Calls:      set_counter()       local function
******************************************************************************
*/

void main()
{
  unsigned read_back, out_byte;
  int aborted = 0;
  IRQ = 10;
  clrscr();
  ADDRESS = AskForBaseAddress (0xFCA0);
  IRQ = AskForBaseIRQ (IRQ);
  printf("\nPress any key to start");
  getch();
  set_counter(); // Set counter and start the countdown
  inportb(ADDRESS+4); //clear any potentially pending interrupts
  initirq(IRQ,setflag);
  outp(ADDRESS+7, 0);   /* Enable counters */

  for (read_back = 0; read_back <=1000; read_back++) {
    CtrMode(ADDRESS, 1, 2); // set counter 1 to mode 2
    CtrLoad(ADDRESS, 1, 0x0010); // load counter 1 with 10 hex
    gotoxy(1, 8);
    printf("Updating counter 1.  Loop number : %hu", read_back);
  }

  puts("\nWaiting for timeout . . .");

  do {
     aborted = kbhit();
  }while ((!(byte & 1 == 1)) && (!aborted) && !flag); // Check bit 0
   //loop while timeout bit is high, no keypress, and no interrupt
  printf("\n\n%4x",byte);//print inportb(ADDRESS+4) from isr
  restoreirq(IRQ);
  puts("");
  if(aborted)
    puts("Timeout aborted!");
  else
    puts("Watchdog timed out successfully.");
  getch();
  inportb(ADDRESS+7);  //disable counters
  outportb(ADDRESS+5,0);  //turn off buzzer
}

