#include "display.h"
#include "io.h"
#include "data_types.h"
#include "common_objects.h"

#include "irq0_15.h"


unsigned int baseadd;
unsigned int iiro16_isr_iiroflag;

void interrupt iiro16_setiiroflag(void)
{
  iiro16_isr_iiroflag = 1;        /* indicate an interrupt has occurred */
  OUTPORTB(baseadd+1,0x00);
  sendEOI();
}   /* end setiiroflag */


/*****************************************************************************
 *  FUNCTION: soft_filter_test -- local routine                              *
 *****************************************************************************/
unsigned int iiro16_soft_filter_test(unsigned int base_address,int df, unsigned char RelayOffset)
{
  char          ch, *valstring;
  unsigned int  failed;
  unsigned char outbyte, inbyte;
  int           count,int_mask,loop,i;
  int           store1[8],store2[8],errorblk[8];

  OUTPORTB(base_address+3,0x00); // disable filters
  OUTPORTB(base_address + RelayOffset,0x00);   // clear all relays
  failed = 0;
  if (df) {
    TEXT_COLOR(LIGHTGRAY);
    CPRINTF("SOFTWARE FILTER TEST\n\r");
  }

  for(int_mask = 0;int_mask < 8; int_mask++) {
    count = 0;
    for(loop=0;loop<5;loop++) {
      outbyte = 1 << int_mask;
      /* Write to relays. */
      OUTPORTB(base_address + RelayOffset,outbyte); // perform a walking bit pattern
      //this loop measures how much time it takes for the readback of relays
      //  to match the output value.
      do {
        count++; //increment counter until data read matches data sent
        inbyte = INPORTB(base_address + RelayOffset+1);// readback
      } while (!(count < 0) && inbyte != outbyte);
      OUTPORTB(base_address + RelayOffset,0);//erase the pattern to prepare for next pass
      DELAY(100);
    } //end for (5 times)
    count /= 5; /* Take the average. */
    store1[int_mask] = count;
    DELAY(5);
  }

  //same loops as above, with filters enabled
  INPORTB(base_address+3);     // enable filters
  for(int_mask = 0;int_mask <8; int_mask++) {
    count = 0;
    for(loop=0;loop<5;loop++) {
      outbyte = 1 << int_mask;
      /* Write to relays. */
      OUTPORTB(base_address + RelayOffset,outbyte);
      do {
        count++; //increment counter until data read matches data sent
        inbyte = INPORTB(base_address + RelayOffset+1);
      } while (!(count < 0) && inbyte != outbyte);
      OUTPORTB(base_address + RelayOffset,0);//erase the pattern to prepare for next pass
      DELAY(100);
    }//end for (5 times)
    count /= 5; /* Take the average. */
    store2[int_mask] = count;
    errorblk[int_mask] = 0;
    if ((store1[int_mask] * 1.5) > store2[int_mask])
      errorblk[int_mask] = 1;
    if ((store1[int_mask] * 3.5) < store2[int_mask])
      errorblk[int_mask] = 2;
    if (errorblk[int_mask]) failed++;
    DELAY(5);
  }//end for (each bit)

  if (df) {
    TEXT_COLOR(LIGHTGRAY);
    CPRINTF("%6s %8s %8s %8s\n\r","Relay","No Filt","Filters","Status");
    for ( i=0 ; i<8 ; i++) {
      TEXT_COLOR((errorblk[i]!=0)?RED:GREEN);
      CPRINTF("%6d %8d %8d %8s\n\r",
              i,
              store1[i],
              store2[i],
              (errorblk[i]==0)?"PASS":(errorblk[i]==1)?"LOW":"HIGH");
      RESETLINE();
    }
  }
  OUTPORTB(base_address+3,0x00);//turn off filters
  OUTPORTB(base_address + RelayOffset,0x00);  //turn off all relays
  return failed;
} /* end soft_filter_test;

/*****************************************************************************
 *  FUNCTION: interrupt_test -- local routine                                *
 *****************************************************************************/
unsigned int iiro16_interrupt_test(unsigned int base_address, unsigned int IRQNumber,int df, unsigned char RelayOffset)
{
  char                ch;
  int                 i,int_mask;
  unsigned long       count;
  void (interrupt far *oldisr)();
  int out;
  int flag = 0;
  int passed = FAILED;
  baseadd=base_address;

  DELAY(100);

  //INPORTB(base_address+1);
  if(!df){
    OUTPORTB(base_address+1,0x00);  //clear pending interrupt
    initirq(IRQNumber,iiro16_setiiroflag);
    sendEOI();
    OUTPORTB(base_address + RelayOffset,0x04); /* Write data to the relay outputs. */
    //INPORTB(base_address+1);
    OUTPORTB(base_address+1,0x00);   //clear pending interrupt
    OUTPORTB(base_address+2,0x00); //disable interrupt
    //INPORTB(base_address+1);
    OUTPORTB(base_address+1,0x00); // Clear the interrupt.
    INPORTB(base_address+2);         //Read to enable interrupts. Writing disables
    iiro16_isr_iiroflag = 0;
    DELAY(10);
    out = 0xFF;
    for(i = 0; i < 8; i++){
       out = out - (1<<i);
       OUTPORTB(base_address + RelayOffset,out); //generate interrupt
       DELAY(1000);
       if (!iiro16_isr_iiroflag)
         passed = FAILED;
         // flag++;
       iiro16_isr_iiroflag = 0;
    }
    //INPORTB(base_address+1);
    OUTPORTB(base_address+1,0x00); // Clear the interrupt.
    OUTPORTB(base_address+2,0x00); //disable interrupt
    //INPORTB(base_address+1);
    OUTPORTB(base_address+1,0x00); // Clear the interrupt.

    sendEOI();                 //clear interrupt to allow next one
    restoreirq(IRQNumber);
  }else{
    TEXT_COLOR(LIGHTGRAY);
    CPRINTF("INTERRUPT TEST\n\r");
    CPRINTF("Relay 0    1        2        3        4        5        6        7\n\r");
    GETCH();
    while( !KBHIT() ) {
      out = 0xff;
      OUTPORTB(base_address+1,0x00);  //clear pending interrupt
      initirq(IRQNumber,iiro16_setiiroflag);
      sendEOI();
      OUTPORTB(base_address + RelayOffset,out); /* Write data to the relay outputs. */
      OUTPORTB(base_address+1,0x00);   //clear pending interrupt
      OUTPORTB(base_address+2,0x00); //disable interrupt
      OUTPORTB(base_address+1,0x00); // Clear the interrupt.
      INPORTB(base_address+2);         //Read to enable interrupts. Writing disables
      iiro16_isr_iiroflag = 0;
      DELAY(10);
      GOTOXY( 3, WHEREY() );
      // GOTOXY( 3 , (stdscr)->_cury );
      for(i = 0; i < 8; i++){
        out = out - (1<<i);
        OUTPORTB(base_address + RelayOffset,out); //generate interrupt
        DELAY(1000);
        if (!iiro16_isr_iiroflag)
        {
          flag++;//how many times did we get no IRQ
          TEXT_COLOR(RED);
          CPRINTF("%-9s", "NoIRQ");
        } else{
          TEXT_COLOR(GREEN);
          CPRINTF("%-9s", "IRQ");
        }
        iiro16_isr_iiroflag = 0;
      }//end for (8 bits)
      OUTPORTB(base_address+1,0x00); // Clear the interrupt.
      OUTPORTB(base_address+2,0x00); //disable interrupt
      OUTPORTB(base_address+1,0x00); // Clear the interrupt.
      sendEOI();                 //clear interrupt to allow next one
      restoreirq(IRQNumber);
    }
    GETCH();
  }
  CPRINTF("\n\r");
  return passed;
} /* end interrupt_test;


/*****************************************************************************
 *  FUNCTION: relay_step_test -- local routine                               *
 *****************************************************************************/
unsigned int iiro16_relay_step_test(unsigned int base_address,int df, unsigned char RelayOffset)
{
  unsigned int  passed,read_in;
  int           readback,outbyte,shiftnum,dataarray[8],dataarray2[8];
  int           last_relay;

  outbyte = shiftnum = 0;
  read_in = TRUE;
  passed = FAILED;
  /* Test to see if board can be written to at all. */
  OUTPORTB(base_address + RelayOffset,0x00);
  DELAY(5);
  outbyte = INPORTB(base_address + RelayOffset);
  if(df){
    TEXT_COLOR(LIGHTGRAY);
    CPRINTF("RELAY STEP TEST\n\r");
  }
  if (outbyte)
   passed = FAILED;
  else {
    for(shiftnum = 0; shiftnum < 8; shiftnum++) {
        outbyte = 1 << shiftnum;
        OUTPORTB(base_address + RelayOffset,outbyte);      //write to each bit
        DELAY(5);
        dataarray2[shiftnum] = INPORTB(base_address + RelayOffset); // Read relay outputs.
        DELAY(10);
        dataarray[shiftnum] = INPORTB(base_address + RelayOffset+1);  // Read isolated inputs.
        if(df)
        {
          TEXT_COLOR((dataarray[shiftnum]==dataarray2[shiftnum])?GREEN:RED);
          CPRINTF("Out Base + 0: %3X    In Base + 0: %3X     In Base + 1: %3X\n\r",outbyte,dataarray2[shiftnum],dataarray[shiftnum]);
          RESETLINE();
        }
        if (dataarray[shiftnum] != dataarray2[shiftnum]) //read-in values should be equal.
           read_in = FALSE;
    }
    if (read_in)
        passed = PASSED;

  }
  return passed;
}  /* end relay_step_test */

/*****************************************************************************
 *  FUNCTION: relay_write_test -- local routine                              *
 *****************************************************************************/

unsigned int iiro16_relay_write_test(unsigned int base_address,int df, unsigned char RelayOffset)
{
  unsigned int passed,readback;
  int          count;

  if(df){
    TEXT_COLOR(LIGHTGRAY);
    CPRINTF("RELAY WRITE TEST\n\r");
  }
  passed = PASSED;;
  readback = 0;

  OUTPORTB(base_address+2,0x00);  //disable interrupts

  /* Start test. First, clear the output relays. */

  OUTPORTB(base_address + RelayOffset,0x00);         /* Clear all output relays */
  DELAY(20);                                         /* Standard 5 millisecond delay */
  readback = INPORTB(base_address + RelayOffset);    /* Read back the relays */

  if (readback)
  {
    passed = FAILED;
    if(df){
      CPRINTF("ZERO TEST FAILED\n\r");
      CPRINTF("The program was unable to zero the output relays.\n\r");
      CPRINTF("Results : %u\n\r",readback);
    }
  }
  OUTPORTB(base_address + RelayOffset,0xFF); //Write 0xFF
  DELAY(20);
  readback = INPORTB(base_address + RelayOffset);
  if (readback != 0xFF)
  {
     passed = FAILED;
     if(df){
       CPRINTF("WRITE TEST FAILED\n\r");
       CPRINTF("The program was unable to write to the relay outputs\n\r");
       CPRINTF("Results :%u",readback);
     }
  }
  return passed;
}  /* end relay_write_test */
