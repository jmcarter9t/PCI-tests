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
#include "pciutil.h"
#include "8254ctr.h"


#include "8254ctr.h"
#include <stdio.h>
unsigned AskForBaseAddress(unsigned int OldOne)
{
	char msg[7];
	int NewOne = 0, Success = 0, Dummy;
	int AddrInputPosX, AddrInputPosY;

	CPRINTF("Please enter the Base Address for your card (in hex)");
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
			printf("%X", OldOne);
			Success = 1;
			Dummy = OldOne;
		}
	} while(!Success);
	return (Dummy);
} /* end of AskForBaseAddress */

unsigned AskForCountAddress(void)
{
	char msg[7];
	int NewOne = 0, Success = 0, Dummy;
	int AddrInputPosX, AddrInputPosY;

        CPRINTF("Please enter the offset for Counter 0 for your card (in hex).");
	CPRINTF("\n>");
	AddrInputPosX = WHEREX(); AddrInputPosY = WHEREY();
	do {     
		GOTOXY(AddrInputPosX, AddrInputPosY);
		CLREOL();
		msg[0] = 5; msg[1] = 0;
		GETS(msg);
		sscanf(msg + 2, "%x", &NewOne);
		Success = 1;
		Dummy = NewOne;
	} while(!Success);
	return (Dummy);
} /* end of AskForCountAddress */

int main(int argc, char *argv[])
{
	unsigned freq=0;
	float time = 0;
	unsigned BASE = 0xFCA0;
	unsigned count = 0;
	CLRSCR();
	BASE=AskForBaseAddress(BASE);
	CLRSCR();
	count=AskForCountAddress(); //get counter offset
	BASE+=count;                //add counter offset to base address
	DELAY(100);
	PUTS("8254 Counter Sample");
	PUTS("--This program measures frequency and pulse width and generates frequency.");
	PUTS("--To measure frequency, connect a source to Clk IN on Pin 33.");
	PUTS("");
	PUTS("Press any key to continue.");
	getch();
	while(!kbhit()){
	  freq = freq;
	  time = frequency_measure(BASE);
	  printf("\nThe frequency is %10.3f\n\n", time);
	  PUTS("Press any key to continue to pulse width test.");
	  GOTOXY(1,WHEREY()-4);
	}
	getch();
	CLRSCR();                //is this correct???
	PUTS("--To measure pulse width, connect a source to Gate 1 on Pin 34.");
	PUTS("--Without a source, pulse width will read zero.");
	PUTS("--Ground Pin 37.");
	PUTS("");
	PUTS("Press any key to continue.");
	getch();
	while(!kbhit()){
	  time = pulse_width(BASE);
	  printf(" Pulse1 %g \n", time);
	  DELAY(1000);
	  time = pulse_width(BASE);
	  printf(" Pulse2 %g \n", time);
	  DELAY(397);
	  time = pulse_width(BASE);
	  printf(" Pulse3 %g \n", time);
	  time = pulse_width(BASE);
	  printf(" Pulse4 %g \n", time);
	  DELAY(300);
	  time = pulse_width(BASE);
	  printf(" Pulse5 %g \n", time);
	  DELAY(10);
	  time = pulse_width(BASE);
	  printf(" Pulse6 %g \n\n", time);
	  PUTS("Press any key to continue to next test.");
	  GOTOXY(1,WHEREY()-8);
  }
  getch();
  CLRSCR();
  while(!kbhit()){
	 PUTS("Generating frequency...");
	 PUTS("Verify signal by connecting a frequency meter to Out 2 on Pin 35.");
	 PUTS("Press any key to continue.");
	 generatefrequency(BASE, 25000);
	 GOTOXY(1,WHEREY()-3);
  }
  getch();
	//DELAY(2000);
}
