#include "display.h"
#include "io.h"
#include "data_types.h"
#include "common_objects.h"
#include "irq.h"
#include "iiro8.h"

// unsigned int baseaddress;
// unsigned int isr_iiroflag;

/* Global variables */
int LatchCode = 0; /* Determines the actions performed by the latch function,
		      do_functions(), based upon which test is in progress. */
int ScreenPosition = 0; /* Used to determine the position of the relay
			   which is being displayed by the latch function. */
int RelayNumber = 0; /* Used to store the relay number of the relay which
			is being displayed on the screen by the latch
			function. */



/*****************************************************************************
 *  FUNCTION: soft_filter_test -- local routine                              *
 *****************************************************************************/
unsigned int soft_filter_test(unsigned int base_address,int df)//SOFTWARE FILTER TEST
{

char                ch, *valstring;
unsigned int        passed;
unsigned char	    outbyte, inbyte;
int                 count,int_mask,loop;
int		    store1[8],store2[8],errorblk[8];

OUTPORTB(base_address+3,0x00); /* base address + 3 */
OUTPORTB(base_address,0x00);
passed = TRUE;
if(df)
  CPRINTF("SOFTWARE FILTER TEST\n\r");
for(int_mask = 0;int_mask <8; int_mask++)
{
  count = 0;
  for(loop=0;loop<5;loop++)
  {
    outbyte = 1 << int_mask;
    /* Write to base address + 0. */
    OUTPORTB(base_address,outbyte);
    do
    {
      count++; //increment counter until data read matches data sent
      inbyte = INPORTB(base_address+1);
    }while (!(count < 0) && inbyte != outbyte);
  }
  count /= 5; /* Take the average. */
  store1[int_mask] = count;
  DELAY(5);
}
  INPORTB(base_address+3);

for(int_mask = 0;int_mask <8; int_mask++)
  {
  count = 0;
  for(loop=0;loop<5;loop++)
    {
    outbyte = 1 << int_mask;
    /* Write to base address + 0. */
    OUTPORTB(base_address,outbyte);
    do
      {
      count++; //increment counter until data read matches data sent
      inbyte = INPORTB(base_address+1);
      }
    while (!(count < 0) && inbyte != outbyte);
    }
  count /= 5; /* Take the average. */
  store2[int_mask] = count;
  errorblk[int_mask] = 0;
  DELAY(5);
  }
if(df){
  TEXT_COLOR(LIGHTGRAY);
  CPRINTF("Relay #     No Filters     Filters\n\r");
}
for(int_mask = 0; int_mask < 8; int_mask++)
{
  if ((store1[int_mask] * 1.5) > store2[int_mask])
  {
    passed = FALSE;
    errorblk[int_mask] = 1;
    if(df){
      TEXT_COLOR(RED);
      CPRINTF("Relay %d     %7d     %8d       LOW\n\r",int_mask,store1[int_mask],store2[int_mask]);
    }
  }
  else
    if ((store1[int_mask] * 3.5) < store2[int_mask])
      {
      passed = FALSE;
      errorblk[int_mask] = 2;
      if(df){
        TEXT_COLOR(RED);
        CPRINTF("Relay %d     %7d     %8d       HIGH\n\r",int_mask,store1[int_mask],store2[int_mask]);

      }
    }
    else{
      errorblk[int_mask] = 0;
      if(df){
        TEXT_COLOR(GREEN);
        CPRINTF("Relay %d     %7d     %8d       PASS\n\r",int_mask,store1[int_mask],store2[int_mask]);
      }
    }
  }

OUTPORTB(base_address+3,0x00);
OUTPORTB(base_address,0x00);
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
unsigned int manual_filter_test(unsigned int base_address,int df)//MANUAL FILTER TEST
{

char                ch, *valstring;
unsigned int        passed;
unsigned char	    outbyte, inbyte;
int                 int_mask,loop;
int   store1[8],store2[8],errorblk[8];
int   count;

if(df){
  TEXT_COLOR(LIGHTGRAY);
  CPRINTF("MANUAL FILTER TEST\n\r");
}
passed = TRUE;
/* disable filter */
OUTPORTB(base_address+3,0x00); /* base address + 3 */
DELAY(10);

OUTPORTB(base_address,0x00);

for(int_mask = 0;int_mask <8; int_mask++)
  {
  count = 0;
  for(loop=0;loop<5;loop++)
    {
    outbyte = 1 << int_mask;
    /* Write to base address + 0. */
    OUTPORTB(base_address,outbyte);
    do
      {
      count++;
      /* Read from base address + 1. Increment counter and read from the
	 card until the data read matches the data sent out. */
      inbyte = INPORTB(base_address+1);
      }
    while (!(count < 0) && inbyte != outbyte); // Safety feature
    }
  count /= 5; /* Take the average. */
  store1[int_mask] = count;
  DELAY(5);
  }
  TEXT_COLOR(LIGHTGRAY);
  CPRINTF("Now enable all manual filters and press any key.\n\r");
  getch();
for(int_mask = 0;int_mask <8; int_mask++)
  {
  count = 0;
  for(loop=0;loop<5;loop++)
    {
    outbyte = 1 << int_mask;
    /* Write to base address + 0. */
    OUTPORTB(base_address,outbyte);
    do
      {
      count++;
      /* Read from base address + 1. Increment counter and read from the
	 card until the data read matches the data sent out. */
      inbyte = INPORTB(base_address+1);
      }
    while (!(count < 0) && inbyte != outbyte); // Safety feature
    }
  count /= 5; /* Take the average. */
  store2[int_mask] = count;
  errorblk[int_mask] = 0;
  DELAY(5);
  }

if(df){
  TEXT_COLOR(LIGHTGRAY);
  CPRINTF("Relay #     No filters     Filters\n\r");
}
for(int_mask = 0; int_mask < 8; int_mask++)
  {
  if ((store1[int_mask] * 1.5) > store2[int_mask])
  {
    passed = FALSE;
    errorblk[int_mask] = 1;
    if(df){
      TEXT_COLOR(RED);
      CPRINTF("Relay %d     %7d     %8d       LOW\n\r",int_mask,store1[int_mask],store2[int_mask]);

    }
  }
  else
    if ((store1[int_mask] * 3.5) < store2[int_mask])
    {
      passed = FALSE;
      errorblk[int_mask] = 2;
      if(df){
        TEXT_COLOR(RED);
        CPRINTF("Relay %d     %7d     %8d       HIGH\n\r",int_mask,store1[int_mask],store2[int_mask]);
      }
    }
    else{
      errorblk[int_mask] = 0;
      if(df){
        TEXT_COLOR(GREEN);
        CPRINTF("Relay %d     %7d     %8d       PASS\n\r",int_mask,store1[int_mask],store2[int_mask]);
      }
    }
  }

OUTPORTB(base_address,0x00);
return passed;
} /* end manual_filter_test;

/*****************************************************************************
 *  FUNCTION: interrupt_test -- local routine                                *
 *****************************************************************************/

unsigned int interrupt_test(unsigned int base_address, unsigned int IRQNumber,int df)
{

char                ch;
unsigned int        passed;
int                 i,int_mask;
unsigned long       count;
void (interrupt far *oldisr)();
int out;
int flag = 0;
baseadd=base_address;

if(df){
  TEXT_COLOR(LIGHTGRAY);
  CPRINTF("INTERRUPT TEST\n\r");
}
DELAY(100);

//INPORTB(base_address+1);
if(!df){
  OUTPORTB(base_address+1,0x00);  //clear pending interrupt
  initirq(IRQNumber,setiiroflag);
  sendEOI();
  OUTPORTB(base_address,0x04); /* Write data to the relay outputs. */
  //INPORTB(base_address+1);
  OUTPORTB(base_address+1,0x00);   //clear pending interrupt
  OUTPORTB(base_address+2,0x00); //disable interrupt
  //INPORTB(base_address+1);
  OUTPORTB(base_address+1,0x00); // Clear the interrupt.
  INPORTB(base_address+2);         //Read to enable interrupts. Writing disables
  iiro_isr_iiroflag = 0;
  passed = FALSE;
  DELAY(10);
  out = 0xFF;
  for(i = 0; i < 8; i++){
     out = out - (1<<i);
     OUTPORTB(base_address,out); //generate interrupt
     DELAY(1000);
     if (!iiro_isr_iiroflag)
       flag++;//passed = TRUE;
     iiro_isr_iiroflag = 0;
  }
//INPORTB(base_address+1);
OUTPORTB(base_address+1,0x00); // Clear the interrupt.
OUTPORTB(base_address+2,0x00); //disable interrupt
//INPORTB(base_address+1);
OUTPORTB(base_address+1,0x00); // Clear the interrupt.

sendEOI();                 //clear interrupt to allow next one
restoreirq(IRQNumber);
}
if(df){
  TEXT_COLOR(LIGHTGRAY);
  CPRINTF("Relay 0    1        2        3        4        5        6        7\n\r");
  while( ! KBHIT() ){
    out = 0xff;
    OUTPORTB(base_address+1,0x00);  //clear pending interrupt
    initirq(IRQNumber,setiiroflag);
    sendEOI();
    OUTPORTB(base_address,out); /* Write data to the relay outputs. */
    OUTPORTB(base_address+1,0x00);   //clear pending interrupt
    OUTPORTB(base_address+2,0x00); //disable interrupt
    OUTPORTB(base_address+1,0x00); // Clear the interrupt.
    INPORTB(base_address+2);         //Read to enable interrupts. Writing disables
    iiro_isr_iiroflag = 0;
    passed = FALSE;
    DELAY(10);
    for(i = 0; i < 8; i++){
      out = out - (1<<i);
      OUTPORTB(base_address,out); //generate interrupt
      DELAY(1000);
      if (!iiro_isr_iiroflag)
        flag++;// passed = TRUE;
      TEXT_COLOR(iiro_isr_iiroflag?GREEN:RED);
      CPRINTF("%s",iiro_isr_iiroflag?"IRQ      ":"NoIRQ    ");
      iiro_isr_iiroflag = 0;
    }
    GOTOXY( 1, WHEREY() );
    OUTPORTB(base_address+1,0x00); // Clear the interrupt.
    OUTPORTB(base_address+2,0x00); //disable interrupt
    OUTPORTB(base_address+1,0x00); // Clear the interrupt.
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
unsigned int relay_step_test(unsigned int base_address,int df)
{

  unsigned int  passed,read_in;
  int           readback,outbyte,shiftnum,dataarray[8],dataarray2[8];
  int           last_relay;

  TEXT_COLOR(LIGHTGRAY);
  CPRINTF("Install an IIRO-8 wrap plug.\n\r");
  CPRINTF("Apply +5V to red wrap plug lead (Relay commons).\n\r");
  CPRINTF("Apply GND to black wrap plug lead (Isolated input lows).\n\r");
  CPRINTF("Disable manual filters, and press any key to start test.\n\r");
  getch();
  outbyte = shiftnum = 0;
  read_in = TRUE;
  passed = TRUE;
  /* Test to see if board can be written to at all. */
  OUTPORTB(base_address,0x00);
  DELAY(5);
  outbyte = INPORTB(base_address);
  if(df){
    TEXT_COLOR(LIGHTGRAY);
    CPRINTF("RELAY STEP TEST\n\r");
  }
  if (outbyte)
   passed = FALSE;
  else
  {
    outbyte =0;
    for(shiftnum = 0; shiftnum < 8; shiftnum++)
    {
        outbyte = 1 << shiftnum;
        OUTPORTB(base_address,outbyte);      //write to each bit
        DELAY(5);
        dataarray2[shiftnum] = INPORTB(base_address); // Read relay outputs.
        DELAY(10);
        dataarray[shiftnum] = INPORTB(base_address+1);  // Read isolated inputs.
        if(df)
        {
          TEXT_COLOR((dataarray[shiftnum]==dataarray2[shiftnum])?GREEN:RED);
          CPRINTF("Out Base + 0: %2x    In Base + 0: %2x     In Base + 1: %2x\n\r",outbyte,dataarray2[shiftnum],dataarray[shiftnum]);
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

unsigned int relay_write_test(unsigned int base_address,int df)
{
  unsigned int       passed,readback;
  int 	      count;

  if(df){
    TEXT_COLOR(LIGHTGRAY);
    CPRINTF("RELAY WRITE TEST\n\r");
  }
  passed = TRUE;
  readback = 0;

  OUTPORTB(base_address+2,0x00);  //disable interrupts

  /* Start test. First, clear the output relays. */

  OUTPORTB(base_address,0x00);         /* Clear all output relays */
  DELAY(5);		       /* Standard 5 millisecond delay */
  readback = INPORTB(base_address);    /* Read back the relays */

  if (readback)
  {
    passed = FALSE;
    if(df){
      CPRINTF("ZERO TEST FAILED\n\r");
      CPRINTF("The program was unable to zero the output relays.\n\r");
      CPRINTF("Results : %u\n\r",readback);
    }
  }
  OUTPORTB(base_address,0xFF); //Write 0xFF
  DELAY(5);
  readback = INPORTB(base_address);
  if (readback != 0xFF)
  {
     passed = FALSE;
     if(df){
       CPRINTF("WRITE TEST FAILED\n\r");
       CPRINTF("The program was unable to write to the relay outputs\n\r");
       CPRINTF("Results :%u",readback);
     }
  }
  return passed;
}  /* end relay_write_test */
