#include "display.h"
#include "io.h"
#include "data_types.h"
#include "common_objects.h"
#include "irq.h"
#include "iiro8.h"
#include "16550.h"
#include "485test.h"

int DebugFlag;
unsigned int Flag96 = 0;
int result = 0;
dword vendev = 0L;
byte bn = 0;                            //bus_number
byte df = 0;                            //device_and_function
byte regnum;
word index;
byte h; //temp values for debug
int foundcount = 0;
word basea, baseb, basec, based, basee, irqnum;

void test_8255();
void test_8255notIEN();
void test_8255IEN();
void test_8254CounterIEN();



void PCIDIO(char *name, int numadd, int D48Flag, int COSFlag, int COUNTFlag, int HDFlag)
{
	result = read_configuration_word(bn, df, regnum = 0x18, &basea);
	basea &= 0xFFFE;
	result += read_configuration_word(bn, df, regnum = 0x3C, &irqnum);
	irqnum &= 0xF;
	if (!result) {
		int i = 0;
		printf("%12s\n\r", name);
		TEXTCOLOR(LIGHTGRAY);
		CPRINTF("Place the tristate jumper in the TST position.\n\r");
		CPRINTF("Press any key to continue.\n\r");
		while (!KBHIT()) ;
		GETCH();


		for (i = 0; i < numadd; i++) {
			result = _8255Tst(basea, DebugFlag);
			TEXTCOLOR(result ? RED : GREEN);
			CPRINTF("     Port %d Tristate %36s\n\r", i, result ? "FAILED" : "PASSED");
			basea += 4;
			result = 0;
		}


		i = 0;
		// TEXTCOLOR(LIGHTGRAY);
		// CPRINTF("Move the TST jumper to the BEN position and install WRAP PLUG.\n\r");
		// CPRINTF("Press any key to continue.\n\r");

		while (!KBHIT()) sleep(1); GETCH();
		result = read_configuration_word(bn, df, regnum = 0x18, &basea);
		basea &= 0xFFFE;
		for (i = 0; i < numadd; i++) {
			// result =  _8255WalkingTest(basea,NODAC,DebugFlag);
			// TEXTCOLOR(result?RED:GREEN);
			// CPRINTF("     Port %d Walking Bit  %37s\n\r",i,result?"FAILED":"PASSED");
			// if(COSFlag) {
			//   result = _8255COS(basea,irqnum,DebugFlag);
			//   TEXTCOLOR(result?RED:GREEN);
			//   CPRINTF("     Port %d COS (IRQ) %40s\n\r",i,result?"FAILED":"PASSED");
			// }

			result  = _8255notIENTest(basea, irqnum, DebugFlag, HDFlag);
			TEXTCOLOR(result ? RED : GREEN);
			CPRINTF("     Port %d Not IRQ %42s\n\r", i, result ? "FAILED" : "PASSED");

			result  = _8255IENTest(basea, irqnum, DebugFlag, D48Flag, COSFlag, HDFlag);
			TEXTCOLOR(result ? RED : GREEN);
			CPRINTF("     Port %d IEN (IRQ) %40s\n\r", i, result ? "FAILED" : "PASSED");

			basea += 4;
			result = 0;
		}

		if (COUNTFlag) {
			result = read_configuration_word(bn, df, regnum = 0x18, &basea);
			basea &= 0xFFFE;

			basec = basea + 0x10;
			if (numadd == 4)
				Flag96 = 1;
			else
				Flag96 = 0;
			result = _8254CounterIEN(basea, irqnum, Flag96, DebugFlag);
			//    result = _8254CounterIEN(basea,irqnum,DebugFlag);
			TEXTCOLOR(result ? RED : GREEN);
			CPRINTF("     Counter Interrupt %39s\n\r", result ? "FAILED" : "PASSED");
			//change back to 2
			for (i = 0; i <= 2; i++) {
				if ((inportb(basec) & inportb(basec + 1) & inportb(basec + 2) & inportb(basec + 3)) != 0xFF) {
					result = _8254Test(basec, DebugFlag);
					TEXTCOLOR(result ? RED : GREEN);
					CPRINTF("     8254 Chip %d %45s\n\r", i, result ? "FAILED" : "PASSED");

				} else{
					TEXTCOLOR(LIGHTGRAY);
					CPRINTF("No counter detected\n\r");
				}
				basec += 4;
			}
		}
	}
}
