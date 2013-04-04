#include "pciutil.h"
#include <curses.h>

/* Configured by cmake */
#include "display.h"
#include "io.h"
#include "common_objects.h"


#include <time.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <stddef.h>
#include <string.h>

#include "8255test.h"
#include "16550.h"
#include "8254.h"

#include "485test.h"
#include "iiro8.h"
#include "iiro16.h"
#include "idio16.h"
#include "dac.h"
#include "ai.h"
#include "idiido.h"
#include "irq0_15.h"
#include "error_codes.h"




#ifdef DOS
void OutPortDW(unsigned Port, unsigned long Value)
{
    asm {
        mov     dx,[Port]
        mov     eax,[Value]
        out     dx,eax
    }
}
#elsif LINUX


#endif



void OTHERINFO()
{ 
  int rn;
  GLOBALS.result = read_configuration_word(GLOBALS.bn,GLOBALS.df,rn = 0x18,&GLOBALS.basea);
  GLOBALS.result = read_configuration_word(GLOBALS.bn,GLOBALS.df,rn = 0x1C,&GLOBALS.baseb);
  GLOBALS.result = read_configuration_word(GLOBALS.bn,GLOBALS.df,rn = 0x20,&GLOBALS.basec);
  GLOBALS.result = read_configuration_word(GLOBALS.bn,GLOBALS.df,rn = 0x24,&GLOBALS.based);
  GLOBALS.result += read_configuration_word(GLOBALS.bn,GLOBALS.df,rn = 0x3C,&GLOBALS.irqnum);
  GLOBALS.basea&=0xFFFE;
  GLOBALS.baseb&=0xFFFE;
  GLOBALS.basec&=0xFFFE;
  GLOBALS.based&=0xFFFE;
  if (!GLOBALS.result){
    printf("A board has been located.\n");
    printf("Base Address A= %X\n",GLOBALS.basea);
    printf("Base Address B= %X\n",GLOBALS.baseb);
    printf("Base Address C= %X\n",GLOBALS.basec);
    printf("Base Address D= %X\n",GLOBALS.based);
    printf("IRQ = %hu\n",GLOBALS.irqnum & 0xF);
  }
  rn=rn;
}




int isr_96flag=0;
int base96=0;

void INTERRUPT setirq96flag(void)
{
  OUTPORT(base96 + 0x10,0x0000);//clear IRQ on card
  // sendEOI();
  isr_96flag = 1;        /* indicate an interrupt has occurred */
}   /* end setiiroflag */


// int _96bitIRQtest(int base,int irq)
// {
//   int ppi,port,bit,bytetowrite,addresstowrite, irqstatus;
//   base96=base;
//   puts("\n\rPLEASE REMOVE THE TEST JIG/WRAP PLUGs and press any key to continue.");
//   GETCH();
//   initirq( irq, setirq96flag );
//   /*
//     0.0 Read COS status register, should be all zeros (no bits were enabled)
//     1.0 Disable all COS bits
//     2.0 Walking bit pattern on all 96 bits, counting IRQs (should be zero)
//     3.0 Enable all COS bits
//     4.0 Walking bit pattern on all 96 bits, counting IRQs (should be 16)
//     5.0 Enable some COS bits
//     6.0 walking bit pattern on all 96 bits, counting IRQs (should be some)
//   */
//   //walking on 96
//   OUTPORTB(base+0,0);
//   OUTPORTB(base+1,0);
//   OUTPORTB(base+2,0);
//   OUTPORTB(base+4,0);
//   OUTPORTB(base+5,0);
//   OUTPORTB(base+6,0);
//   OUTPORTB(base+8,0);
//   OUTPORTB(base+9,0);
//   OUTPORTB(base+0xa,0);
//   OUTPORTB(base+0xc,0);
//   OUTPORTB(base+0xd,0);
//   OUTPORTB(base+0xe,0);

//   if (INPORT(base+0x10) != 0x0000)
//     puts("Error, unexpected IRQ status - power on enabled??");
//   OUTPORT(base+0x12,0x0000);//disable all COS IRQs
//   puts("COS disabled");

//   OUTPORTB(base+3,0x80);
//   OUTPORTB(base+7,0x80);
//   OUTPORTB(base+0x0B,0x80);
//   OUTPORTB(base+0x0F,0x80);

//   puts("Walking bit, should get no IRQs");
//   for (ppi=0;ppi<4;ppi++)
//     for (port=0;port<3;port++)
//       for (bit=-1;bit<9;bit++)
//       {
//         if ((bit == -1) || (bit == 8))  //start with "0", and end with "0"
//           bytetowrite = 0;
//         else
//           bytetowrite = (1 << bit);
//         addresstowrite = base + (ppi * 4) + port;

//         isr_96flag=0;
//         OUTPORTB(addresstowrite,bytetowrite);

//         DELAY(1);
//         if (isr_96flag)
//         {
//           printf("BAD!! IRQ on PPI %d, port %d, bit %d\n\r",ppi,port,bit);
//           return 1;
//         }
//       }
//   irqstatus = INPORT(base+0x10);
//   if ( irqstatus != 0x0000)
//     printf("\n\rError, unexpected IRQ status %04x should be 0000\n\r",irqstatus);

//   OUTPORT(base+0x12,0xffff);//enable all COS IRQs
//   puts("COS enabled all bits (16 total)");
//   puts("Walking bit, should get 1 IRQ each on all PPIs, port 2 bits 4-7");
//   for (ppi=0;ppi<4;ppi++)
//     for (port=0;port<3;port++)
//       for (bit=-1;bit<9;bit++)
//       {
//         if ((bit == -1) || (bit == 8))  //start with "0", and end with "0"
//           bytetowrite = 0;
//         else
//           bytetowrite = (1 << bit);
//         addresstowrite = base + (ppi * 4) + port;

//         isr_96flag=0;
//         OUTPORTB(addresstowrite,bytetowrite);

//         DELAY(1);
//         if ((isr_96flag) && (bit != 8))  //don't both showing the falling edge
//           printf("IRQ on PPI %d, port %d, bit %d\n\r",ppi,port,bit);
//       }
//   irqstatus = INPORT(base+0x10);

//   OUTPORT(base+0x12,0x0000);
//   OUTPORTB(base+3,0x9b);
//   OUTPORTB(base+7,0x9b);
//   OUTPORTB(base+0x0B,0x9b);
//   OUTPORTB(base+0x0F,0x9b);

//   restoreirq(irq);
//   return 0;
// }

void pci_IIRO(char *name)
{

  GLOBALS.result = read_configuration_word(GLOBALS.bn,GLOBALS.df,GLOBALS.regnum = 0x14,&GLOBALS.basea); //enable IRQ generation
  GLOBALS.basea &= 0xFFFE;
  OUTPORTB(GLOBALS.basea+0x69,0x09);


  GLOBALS.result = read_configuration_word(GLOBALS.bn, GLOBALS.df, GLOBALS.regnum = 0x18, &GLOBALS.basea);
  GLOBALS.basea  &=  0xFFFE;
  GLOBALS.result += read_configuration_word(GLOBALS.bn, GLOBALS.df, GLOBALS.regnum = 0x3C, &GLOBALS.irqnum);
  GLOBALS.irqnum  &=  0xF;
  if (!GLOBALS.result){
    //test if card found
    TEXT_COLOR(LIGHTGRAY);
    PRINTF("%40s\n\r",name);
    TEXT_COLOR(GLOBALS.result?RED:GREEN);
    PRINTF("BASE:%04X  IRQ:%hu%40s\n\r",GLOBALS.basea, GLOBALS.irqnum, GLOBALS.result?"FAILED":"PASSED");

    GLOBALS.result = relay_write_test(GLOBALS.basea,GLOBALS.DebugFlag);
    TEXT_COLOR( GLOBALS.result ? RED : GREEN);
    PRINTF("      Relay Write Test: %47s\n\r",GLOBALS.result?"FAILED":"PASSED");

    GLOBALS.result = relay_step_test(GLOBALS.basea,GLOBALS.DebugFlag);
    TEXT_COLOR(GLOBALS.result?RED:GREEN);
    PRINTF("      Relay Step Test: %48s\n\r",GLOBALS.result?"FAILED":"PASSED");

    GLOBALS.result = soft_filter_test(GLOBALS.basea,GLOBALS.DebugFlag);
    TEXT_COLOR(GLOBALS.result?RED:GREEN);
    PRINTF("      Software Filter Test: %43s\n\r",GLOBALS.result?"FAILED":"PASSED");

    GLOBALS.result = manual_filter_test(GLOBALS.basea,GLOBALS.DebugFlag);
    TEXT_COLOR(GLOBALS.result?RED:GREEN);
    PRINTF("      Manual Filter Test: %45s\n\r",GLOBALS.result?"FAILED":"PASSED");

    GLOBALS.result = interrupt_test(GLOBALS.basea,GLOBALS.irqnum,GLOBALS.DebugFlag);
    TEXT_COLOR(GLOBALS.result?RED:GREEN);
    PRINTF("      Interrupt Test: %49s\n\r",GLOBALS.result?"FAILED":"PASSED");
  }

}

void pci_IIRO16(char *name)
{

  GLOBALS.result = read_configuration_word(GLOBALS.bn,GLOBALS.df,GLOBALS.regnum = 0x14,&GLOBALS.basea); //enable IRQ generation
  GLOBALS.basea &= 0xFFFE;
  OUTPORTB(GLOBALS.basea+0x69,0x09);

  GLOBALS.result = read_configuration_word(GLOBALS.bn,GLOBALS.df,GLOBALS.regnum = 0x18,&GLOBALS.basea);
  GLOBALS.basea  &=  0xFFFE;
  GLOBALS.result += read_configuration_word(GLOBALS.bn,GLOBALS.df,GLOBALS.regnum = 0x3C,&GLOBALS.irqnum);
  GLOBALS.irqnum  &=  0xF;
  if (!GLOBALS.result){
    //test if card found
    TEXT_COLOR(LIGHTGRAY);
    PRINTF("%40s\n\r",name);
    TEXT_COLOR(GLOBALS.result?RED:GREEN);
    PRINTF("BASE:%04X  IRQ:%hu%40s\n\r",GLOBALS.basea, GLOBALS.irqnum, GLOBALS.result?"FAILED":"PASSED");

    unsigned char RelayOffset = 8;
    do
    {
      RelayOffset -= 4;

      TEXT_COLOR(LIGHTGRAY);
      PRINTF("   %s Port\n\r", RelayOffset ? "HI" : "LO");
      PRINTF("Install an IIRO-16 spider cable with an IIRO-16 wrap plug on the %s port.\n\r", RelayOffset ? "HI" : "LO");
      PRINTF("Apply +5V to red wrap plug lead (Relay commons).\n\r");
      PRINTF("Apply GND to black wrap plug lead (Isolated input lows).\n\r");
      PRINTF("Press any key to start test.\n\r");
      GETCH();

      GLOBALS.result = iiro16_relay_write_test(GLOBALS.basea,GLOBALS.DebugFlag, RelayOffset);
      TEXT_COLOR(GLOBALS.result?RED:GREEN);
      PRINTF("      Relay Write Test: %47s\n\r",GLOBALS.result?"FAILED":"PASSED");

      GLOBALS.result = iiro16_relay_step_test(GLOBALS.basea,GLOBALS.DebugFlag, RelayOffset);
      TEXT_COLOR(GLOBALS.result?RED:GREEN);
      PRINTF("      Relay Step Test: %48s\n\r",GLOBALS.result?"FAILED":"PASSED");

      GLOBALS.result = iiro16_soft_filter_test(GLOBALS.basea,GLOBALS.DebugFlag, RelayOffset);
      TEXT_COLOR(GLOBALS.result?RED:GREEN);
      PRINTF("      Software Filter Test: %43s\n\r",GLOBALS.result?"FAILED":"PASSED");

      GLOBALS.result = iiro16_interrupt_test(GLOBALS.basea,GLOBALS.irqnum,GLOBALS.DebugFlag, RelayOffset);
      TEXT_COLOR(GLOBALS.result?RED:GREEN);
      PRINTF("      Interrupt Test: %49s\n\r",GLOBALS.result?"FAILED":"PASSED");
    } while (RelayOffset);
  }
}

void ShowHelp(void)
{
  CLRSCR();
  TEXT_COLOR(LIGHTGRAY);
  PRINTF("PCI-Card Test Program\n\r");
  PRINTF("--This program tests PCI cards and is designed \n\r");
  PRINTF("  to run with as few keystrokes as possible.\n\r");
  PRINTF("--Place wrap plugs on all ports and install the PCI card.\n\r");
  PRINTF("--Run the program by typing 'pcietest' in DOS.\n\r");
  PRINTF("--The program detects the card and runs the appropriate tests.\n\r");
  PRINTF("--The GLOBALS.results of each test are displayed in green or red, green\n\r");
  PRINTF("  means pass and red means fail.\n\r");
  PRINTF("\n\r");
  PRINTF("--For COM boards, install a single 422 wrap plug before running the program.\n\r");
  PRINTF("--The 232 test will fail the first time. Move the jumpers to 232,\n\r");
  PRINTF("  install a single 232 wrap plug, and run the program again to test 232.\n\r");
  PRINTF("--For the IRQ test, install the AUTO and ECHO jumpers for each port.\n\r");
  PRINTF("\n\r");
  PRINTF("--To run debug mode, type 'pcietest /d' at the command prompt.\n\r");
  PRINTF("--The colors indicating pass or fail are also present in debug mode.\n\r");
  PRINTF("--Debug displays the values generated during each test, which\n\r");
  PRINTF("  runs until a key is pressed, moving to the next test.\n\r");

  PRINTF("--Press any key to continue test.\n\r");
  GETCH();
}

int main(int argc, char *argv[])
{
  unsigned short IDIAddr = 0, IDOAddr = 0;
  unsigned char IDIIRQ;
  unsigned short i;

  int bIDOverflow = FALSE, bIDIMini = FALSE, bIDOMini = FALSE;
  unsigned int numcards = GLOBALS.list_size;
// (sizeof( struct TCardList ));
  for (i=0;i<argc;i++){
    if (!strcmp(argv[i],"/?")) ShowHelp();
    if (!strcmp(argv[i],"/help")) ShowHelp();
    if (!strcmp(argv[i],"/d")) GLOBALS.df=TRUE;
   // if (!strcmp(arg[i],"/t")) GLOBALS.df=4;  //for software test

  }

  GLOBALS.DebugFlag=GLOBALS.df;//LoadParamsFromCommandLine(argc, *arg);

  CLRSCR();
  TEXT_COLOR(LIGHTGRAY);
  printf("PCIeTest 0.30");if (GLOBALS.df) printf(" running in DEBUG mode.");
  printf("\n");
  GLOBALS.result = !pci_bios_present(&GLOBALS.h,&i,&GLOBALS.bn);
  if (GLOBALS.result){
    PRINTF("Number of PCI Busses Present: %u\n\r",GLOBALS.bn+1);
    i=0;
    do{
      if( find_pci_device( list[i].dev, 0x494f, GLOBALS.indexvalue ,&GLOBALS.bn, &GLOBALS.df )==SUCCESSFUL){
        if (GLOBALS.foundcount>0) {
          puts("Please press any key to view the next card's information.");
          GETCH();
          CLRSCR();
          printf("PCIeTest 0.30");if (GLOBALS.df) printf(" running in DEBUG mode.");
          printf("\n");
        }
        switch(list[i].type){

        case IIRO8:         
          pci_IIRO( list[i].cn );
          break;
        case IIRO16:        
          pci_IIRO16( list[i].cn );
          break;
        default:
          printf("The %lx device detected is not recognized by this program.\n"
                 "It is either newer than this program, or provided by some other\n"
                 "vendor.  Please get the latest version of PCIeTest, and retry.\n",GLOBALS.vendev);
          GLOBALS.foundcount--;
          break;
        }
        GLOBALS.foundcount ++;
        GLOBALS.indexvalue ++;
      } else {
        i++;
        GLOBALS.indexvalue = 0;
      }
    } while(i<numcards);

    TEXT_COLOR(LIGHTGRAY);

    if (GLOBALS.foundcount) {
      if (GLOBALS.foundcount>1)
        PRINTF("A total of %d cards have been found.\r\n",GLOBALS.foundcount);
      else
        PRINTF("\r\nOne card was found.\r\n");
    } else {
      PRINTF("None of the PCI cards under consideration have been found.\r\nMake sure your card is installed.\r\n");
      if( find_pci_device(0x9050,0x10b5,0,&GLOBALS.bn,&GLOBALS.df)==SUCCESSFUL) PRINTF("A PLX 9050 card *was* found.\r\n");
      if( find_pci_device(0x9030,0x10b5,0,&GLOBALS.bn,&GLOBALS.df)==SUCCESSFUL) PRINTF("A PLX 9030 card *was* found.\r\n");
    }
  } else {
    PRINTF("No PCI Bus compliant BIOS was found!\r\n");
  }
  PRINTF("Exiting.\r\n");
  ENDWIN();
  return 0;
}
