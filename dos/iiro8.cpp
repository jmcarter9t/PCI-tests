#include <stdio.h>
#include <bios.h>
#include <conio.h>
#include <dos.h>
#include "irq0-15.h"
#define FALSE 1
#define TRUE 0

unsigned int baseaddress;

/* Global variables */
int LatchCode = 0; /* Determines the actions performed by the latch function,
		      do_functions(), based upon which test is in progress. */
int ScreenPosition = 0; /* Used to determine the position of the relay
			   which is being displayed by the latch function. */
int RelayNumber = 0; /* Used to store the relay number of the relay which
			is being displayed on the screen by the latch
			function. */
unsigned int isr_iiroflag;

void interrupt setiiroflag(...)
{
//inportb(baseaddress+1);

outportb(baseaddress+1,0x00);
sendEOI();
isr_iiroflag = 1;        /* indicate an interrupt has occurred */
}   /* end setiiroflag */

/*****************************************************************************
 *  FUNCTION: soft_filter_test -- local routine                              *
 *****************************************************************************/
unsigned int soft_filter_test(unsigned int baddress,int df)//SOFTWARE FILTER TEST
{

char                ch, *valstring;
unsigned int        passed;
unsigned char	    outbyte, inbyte;
int                 count,int_mask,loop;
int		    store1[8],store2[8],errorblk[8];

outportb(baddress+3,0x00); /* base address + 3 */
outportb(baddress,0x00);
passed = TRUE;
if(df)
  cprintf("SOFTWARE FILTER TEST\n\r");
for(int_mask = 0;int_mask <8; int_mask++)
{
  count = 0;
  for(loop=0;loop<5;loop++)
  {
    outbyte = 1 << int_mask;
    /* Write to base address + 0. */
    outportb(baddress,outbyte);
    do
    {
      count++; //increment counter until data read matches data sent
      inbyte = inportb(baddress+1);
    }while (!(count < 0) && inbyte != outbyte);
  }
  count /= 5; /* Take the average. */
  store1[int_mask] = count;
  delay(5);
}
  inportb(baddress+3);

for(int_mask = 0;int_mask <8; int_mask++)
  {
  count = 0;
  for(loop=0;loop<5;loop++)
    {
    outbyte = 1 << int_mask;
    /* Write to base address + 0. */
    outportb(baddress,outbyte);
    do
      {
      count++; //increment counter until data read matches data sent
      inbyte = inportb(baddress+1);
      }
    while (!(count < 0) && inbyte != outbyte);
    }
  count /= 5; /* Take the average. */
  store2[int_mask] = count;
  errorblk[int_mask] = 0;
  delay(5);
  }
if(df){
  textcolor(LIGHTGRAY);
  cprintf("Relay #     No Filters     Filters\n\r");
}
for(int_mask = 0; int_mask < 8; int_mask++)
{
  if ((store1[int_mask] * 1.5) > store2[int_mask])
  {
    passed = FALSE;
    errorblk[int_mask] = 1;
    if(df){
      textcolor(RED);
      cprintf("Relay %d     %7d     %8d       LOW\n\r",int_mask,store1[int_mask],store2[int_mask]);
    }
  }
  else
    if ((store1[int_mask] * 3.5) < store2[int_mask])
      {
      passed = FALSE;
      errorblk[int_mask] = 2;
      if(df){
        textcolor(RED);
        cprintf("Relay %d     %7d     %8d       HIGH\n\r",int_mask,store1[int_mask],store2[int_mask]);

      }
    }
    else{
      errorblk[int_mask] = 0;
      if(df){
        textcolor(GREEN);
        cprintf("Relay %d     %7d     %8d       PASS\n\r",int_mask,store1[int_mask],store2[int_mask]);
      }
    }
  }

outportb(baddress+3,0x00);
outportb(baddress,0x00);
return passed;

} /* end soft_filter_test;


/*****************************************************************************
 *  FUNCTION: manual_filter_test -- local routine                            *
 *                                                                           *
 *   PURPOSE: This test is designed to test the manual filters. When turned  *
 *            on, the manual filters cause a slight delay before the data is *
 *            available at base address + 1. This test writes to base address*
 *            + 0, and then starts counting until valid data is read at the  *
 *            next address. The count is noticeably lower for channels where *
 *            manual filters are not on. The user determines the success or  *
 *            failure of this test through interpretation of the data.	     *
 *                                                                         *
 *****************************************************************************/
unsigned int manual_filter_test(unsigned int baddress,int df)//MANUAL FILTER TEST
{

char                ch, *valstring;
unsigned int        passed;
unsigned char	    outbyte, inbyte;
int                 int_mask,loop;
int   store1[8],store2[8],errorblk[8];
int   count;

if(df){
  textcolor(LIGHTGRAY);
  cprintf("MANUAL FILTER TEST\n\r");
}
passed = TRUE;
/* disable filter */
outportb(baddress+3,0x00); /* base address + 3 */
delay(10);

outportb(baddress,0x00);

for(int_mask = 0;int_mask <8; int_mask++)
  {
  count = 0;
  for(loop=0;loop<5;loop++)
    {
    outbyte = 1 << int_mask;
    /* Write to base address + 0. */
    outportb(baddress,outbyte);
    do
      {
      count++;
      /* Read from base address + 1. Increment counter and read from the
	 card until the data read matches the data sent out. */
      inbyte = inportb(baddress+1);
      }
    while (!(count < 0) && inbyte != outbyte); // Safety feature
    }
  count /= 5; /* Take the average. */
  store1[int_mask] = count;
  delay(5);
  }
  textcolor(LIGHTGRAY);
  cprintf("Now enable all manual filters and press any key.\n\r");
  getch();
for(int_mask = 0;int_mask <8; int_mask++)
  {
  count = 0;
  for(loop=0;loop<5;loop++)
    {
    outbyte = 1 << int_mask;
    /* Write to base address + 0. */
    outportb(baddress,outbyte);
    do
      {
      count++;
      /* Read from base address + 1. Increment counter and read from the
	 card until the data read matches the data sent out. */
      inbyte = inportb(baddress+1);
      }
    while (!(count < 0) && inbyte != outbyte); // Safety feature
    }
  count /= 5; /* Take the average. */
  store2[int_mask] = count;
  errorblk[int_mask] = 0;
  delay(5);
  }

if(df){
  textcolor(LIGHTGRAY);
  cprintf("Relay #     No filters     Filters\n\r");
}
for(int_mask = 0; int_mask < 8; int_mask++)
  {
  if ((store1[int_mask] * 1.5) > store2[int_mask])
  {
    passed = FALSE;
    errorblk[int_mask] = 1;
    if(df){
      textcolor(RED);
      cprintf("Relay %d     %7d     %8d       LOW\n\r",int_mask,store1[int_mask],store2[int_mask]);

    }
  }
  else
    if ((store1[int_mask] * 3.5) < store2[int_mask])
    {
      passed = FALSE;
      errorblk[int_mask] = 2;
      if(df){
        textcolor(RED);
        cprintf("Relay %d     %7d     %8d       HIGH\n\r",int_mask,store1[int_mask],store2[int_mask]);
      }
    }
    else{
      errorblk[int_mask] = 0;
      if(df){
        textcolor(GREEN);
        cprintf("Relay %d     %7d     %8d       PASS\n\r",int_mask,store1[int_mask],store2[int_mask]);
      }
    }
  }

outportb(baddress,0x00);
return passed;
} /* end manual_filter_test;

/*****************************************************************************
 *  FUNCTION: interrupt_test -- local routine                                *
 *****************************************************************************/

unsigned int interrupt_test(unsigned int baddress, unsigned int IRQNumber,int df)
{

char                ch;
unsigned int        passed;
int                 i,int_mask;
unsigned long       count;
void (interrupt far *oldisr)();
int out;
int flag = 0;
baseaddress=baddress;

if(df){
  textcolor(LIGHTGRAY);
  cprintf("INTERRUPT TEST\n\r");
}
delay(100);

//inportb(baddress+1);
if(!df){
  outportb(baddress+1,0x00);  //clear pending interrupt
  initirq(IRQNumber,setiiroflag);
  sendEOI();
  outportb(baddress,0x04); /* Write data to the relay outputs. */
  //inportb(baddress+1);
  outportb(baddress+1,0x00);   //clear pending interrupt
  outportb(baddress+2,0x00); //disable interrupt
  //inportb(baddress+1);
  outportb(baddress+1,0x00); // Clear the interrupt.
  inportb(baddress+2);         //Read to enable interrupts. Writing disables
  isr_iiroflag = 0;
  passed = FALSE;
  delay(10);
  out = 0xFF;
  for(i = 0; i < 8; i++){
     out = out - (1<<i);
     outportb(baddress,out); //generate interrupt
     delay(1000);
     if (!isr_iiroflag)
       flag++;//passed = TRUE;
     isr_iiroflag = 0;
  }
//inportb(baddress+1);
outportb(baddress+1,0x00); // Clear the interrupt.
outportb(baddress+2,0x00); //disable interrupt
//inportb(baddress+1);
outportb(baddress+1,0x00); // Clear the interrupt.

sendEOI();                 //clear interrupt to allow next one
restoreirq(IRQNumber);
}
if(df){
  textcolor(LIGHTGRAY);
  cprintf("Relay 0    1        2        3        4        5        6        7\n\r");
  while(!kbhit()){
    out = 0xff;
    outportb(baddress+1,0x00);  //clear pending interrupt
    initirq(IRQNumber,setiiroflag);
    sendEOI();
    outportb(baddress,out); /* Write data to the relay outputs. */
    outportb(baddress+1,0x00);   //clear pending interrupt
    outportb(baddress+2,0x00); //disable interrupt
    outportb(baddress+1,0x00); // Clear the interrupt.
    inportb(baddress+2);         //Read to enable interrupts. Writing disables
    isr_iiroflag = 0;
    passed = FALSE;
    delay(10);
    for(i = 0; i < 8; i++){
      out = out - (1<<i);
      outportb(baddress,out); //generate interrupt
      delay(1000);
      if (!isr_iiroflag)
        flag++;// passed = TRUE;
      textcolor(isr_iiroflag?GREEN:RED);
      cprintf("%s",isr_iiroflag?"IRQ      ":"NoIRQ    ");
      isr_iiroflag = 0;
    }
    gotoxy(1,wherey());
    outportb(baddress+1,0x00); // Clear the interrupt.
    outportb(baddress+2,0x00); //disable interrupt
    outportb(baddress+1,0x00); // Clear the interrupt.
    sendEOI();                 //clear interrupt to allow next one
    restoreirq(IRQNumber);
  }
  printf("\n");
  getch();
}
return flag;
} /* end interrupt_test;


/*****************************************************************************
 *  FUNCTION: relay_step_test -- local routine                               *
 *****************************************************************************/
unsigned int relay_step_test(unsigned int baddress,int df)
{

  unsigned int  passed,read_in;
  int           readback,outbyte,shiftnum,dataarray[8],dataarray2[8];
  int           last_relay;

  textcolor(LIGHTGRAY);
  cprintf("Install an IIRO-8 wrap plug.\n\r");
  cprintf("Apply +5V to red wrap plug lead (Relay commons).\n\r");
  cprintf("Apply GND to black wrap plug lead (Isolated input lows).\n\r");
  cprintf("Disable manual filters, and press any key to start test.\n\r");
  getch();
  outbyte = shiftnum = 0;
  read_in = TRUE;
  passed = TRUE;
  /* Test to see if board can be written to at all. */
  outportb(baddress,0x00);
  delay(5);
  outbyte = inportb(baddress);
  if(df){
    textcolor(LIGHTGRAY);
    cprintf("RELAY STEP TEST\n\r");
  }
  if (outbyte)
   passed = FALSE;
  else
  {
    outbyte =0;
    for(shiftnum = 0; shiftnum < 8; shiftnum++)
    {
        outbyte = 1 << shiftnum;
        outportb(baddress,outbyte);      //write to each bit
        delay(5);
        dataarray2[shiftnum] = inportb(baddress); // Read relay outputs.
        delay(10);
        dataarray[shiftnum] = inportb(baddress+1);  // Read isolated inputs.
        if(df)
        {
          textcolor((dataarray[shiftnum]==dataarray2[shiftnum])?GREEN:RED);
          cprintf("Out Base + 0: %2x    In Base + 0: %2x     In Base + 1: %2x\n\r",outbyte,dataarray2[shiftnum],dataarray[shiftnum]);
        }
        if (dataarray[shiftnum] != dataarray2[shiftnum]) //read-in values should be equal.
           read_in = FALSE;
    }
    if (read_in)
        passed = FALSE;

  }
  return passed;
}  /* end relay_step_test */

/*****************************************************************************
 *  FUNCTION: relay_write_test -- local routine                              *
 *****************************************************************************/

unsigned int relay_write_test(unsigned int baddress,int df)
{
  unsigned int       passed,readback;
  int 	      count;

  if(df){
    textcolor(LIGHTGRAY);
    cprintf("RELAY WRITE TEST\n\r");
  }
  passed = TRUE;
  readback = 0;

  outportb(baddress+2,0x00);  //disable interrupts

  /* Start test. First, clear the output relays. */

  outportb(baddress,0x00);         /* Clear all output relays */
  delay(5);		       /* Standard 5 millisecond delay */
  readback = inportb(baddress);    /* Read back the relays */

  if (readback)
  {
    passed = FALSE;
    if(df){
      cprintf("ZERO TEST FAILED\n\r");
      cprintf("The program was unable to zero the output relays.\n\r");
      cprintf("Results : %u\n\r",readback);
    }
  }
  outportb(baddress,0xFF); //Write 0xFF
  delay(5);
  readback = inportb(baddress);
  if (readback != 0xFF)
  {
     passed = FALSE;
     if(df){
       cprintf("WRITE TEST FAILED\n\r");
       cprintf("The program was unable to write to the relay outputs\n\r");
       cprintf("Results :%u",readback);
     }
  }
  return passed;
}  /* end relay_write_test */
