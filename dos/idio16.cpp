#include <stdio.h>
#include <bios.h>
#include <conio.h>
#include <dos.h>
#include "irq0-15.h"
#define FALSE 1
#define TRUE 0

unsigned int idiobaseadd;

unsigned int idio16_isr_idioflag;

void interrupt idio16_setidioflag(...)
{
  idio16_isr_idioflag = 1;        /* indicate an interrupt has occurred */
  outportb(idiobaseadd+1,0x00);
  sendEOI();
}   /* end setiiroflag */


/*****************************************************************************
 *  FUNCTION: soft_filter_test -- local routine                              *
 *****************************************************************************/
unsigned int idio16_soft_filter_test(unsigned int baddress,int df, unsigned char RelayOffset)//SOFTWARE FILTER TEST
{
  char          ch, *valstring;
  unsigned int  failed;
  unsigned char outbyte, inbyte;
  int           count,int_mask,loop,i;
  int           store1[8],store2[8],errorblk[8];

  outportb(baddress+3,0x00); // disable filters
  outportb(baddress + RelayOffset,0x00);   // clear all relays
  failed = 0;
  if (df)
  {
    textcolor(LIGHTGRAY);
    cprintf("SOFTWARE FILTER TEST\n\r");
  }

  for(int_mask = 0;int_mask < 8; int_mask++)
  {
    count = 0;
    for(loop=0;loop<5;loop++)
    {
      outbyte = 1 << int_mask;
      /* Write to relays. */
      outportb(baddress + RelayOffset,outbyte); // perform a walking bit pattern
  //this loop measures how much time it takes for the readback of relays
  //  to match the output value.
      do
      {
        count++; //increment counter until data read matches data sent
        inbyte = inportb(baddress + RelayOffset+1);// readback
      }while (!(count < 0) && inbyte != outbyte);
      outportb(baddress + RelayOffset,0);//erase the pattern to prepare for next pass
      delay(100);
    }//end for (5 times)
    count /= 5; /* Take the average. */
    store1[int_mask] = count;
    delay(5);
  }//end for (each bit)

  //same loops as above, with filters enabled
  inportb(baddress+3);     // enable filters
  for(int_mask = 0;int_mask <8; int_mask++)
  {
    count = 0;
    for(loop=0;loop<5;loop++)
    {
      outbyte = 1 << int_mask;
      /* Write to relays. */
      outportb(baddress + RelayOffset,outbyte);
      do
      {
        count++; //increment counter until data read matches data sent
        inbyte = inportb(baddress + RelayOffset+1);
      }while (!(count < 0) && inbyte != outbyte);
      outportb(baddress + RelayOffset,0);//erase the pattern to prepare for next pass
      delay(100);
    }//end for (5 times)
    count /= 5; /* Take the average. */
    store2[int_mask] = count;
    errorblk[int_mask] = 0;
    if ((store1[int_mask] * 60) > store2[int_mask])
      errorblk[int_mask] = 1;
    if ((store1[int_mask] * 140) < store2[int_mask])
      errorblk[int_mask] = 2;
    if (errorblk[int_mask]) failed++;
    delay(5);
  }//end for (each bit)

  if (df){
    textcolor(LIGHTGRAY);
    cprintf("%6s %8s %8s %8s\n\r","Relay","No Filt","Filters","Status");
    for(i=0;i<8;i++)
    {
      textcolor((errorblk[i]!=0)?RED:GREEN);
      cprintf("%6d %8d %8d %8s\n\r",
              i,
              store1[i],
              store2[i],
              (errorblk[i]==0)?"PASS":(errorblk[i]==1)?"LOW":"HIGH");
    }
  }
  outportb(baddress+3,0x00);//turn off filters
  outportb(baddress + RelayOffset,0x00);  //turn off all relays
  return failed;
} /* end soft_filter_test */

/*****************************************************************************
 *  FUNCTION: interrupt_test -- local routine                                *
 *****************************************************************************/
unsigned int idio16_interrupt_test(unsigned int baddress, unsigned int IRQNumber,int df, unsigned char RelayOffset)
{
  char                ch;
  int                 i,int_mask;
  unsigned long       count;
  void (interrupt far *oldisr)();
  int out;
  int flag = 0;
  idiobaseadd=baddress;

  delay(100);

  //inportb(baddress+1);
  if(!df){
    outportb(baddress+1,0x00);  //clear pending interrupt
    initirq(IRQNumber,idio16_setidioflag);
    sendEOI();
    outportb(baddress + RelayOffset,0x04); /* Write data to the relay outputs. */
    //inportb(baddress+1);
    outportb(baddress+1,0x00);   //clear pending interrupt
    outportb(baddress+2,0x00); //disable interrupt
    //inportb(baddress+1);
    outportb(baddress+1,0x00); // Clear the interrupt.
    inportb(baddress+2);         //Read to enable interrupts. Writing disables
    idio16_isr_idioflag = 0;
    delay(10);
    out = 0xFF;
    for(i = 0; i < 8; i++){
       out = out - (1<<i);
       outportb(baddress + RelayOffset,out); //generate interrupt
       delay(500);
       if (!idio16_isr_idioflag)
         flag++;//passed = FALSE;
       idio16_isr_idioflag = 0;
    }
    //inportb(baddress+1);
    outportb(baddress+1,0x00); // Clear the interrupt.
    outportb(baddress+2,0x00); //disable interrupt
    //inportb(baddress+1);
    outportb(baddress+1,0x00); // Clear the interrupt.

    sendEOI();                 //clear interrupt to allow next one
    restoreirq(IRQNumber);
  }else{
    textcolor(LIGHTGRAY);
    cprintf("INTERRUPT TEST\n\r");
    cprintf("Relay 0    1        2        3        4        5        6        7\n\r");
    while(!kbhit()){
      out = 0xff;
      outportb(baddress+1,0x00);  //clear pending interrupt
      initirq(IRQNumber,idio16_setidioflag);
      sendEOI();
      outportb(baddress + RelayOffset,out); /* Write data to the relay outputs. */
      outportb(baddress+1,0x00);   //clear pending interrupt
      outportb(baddress+2,0x00); //disable interrupt
      outportb(baddress+1,0x00); // Clear the interrupt.
      inportb(baddress+2);         //Read to enable interrupts. Writing disables
      idio16_isr_idioflag = 0;
      delay(10);
      gotoxy(3,wherey());
      for(i = 0; i < 8; i++){
        out = out - (1<<i);
        outportb(baddress + RelayOffset,out); //generate interrupt
        delay(500);
        if (!idio16_isr_idioflag)
        {
          flag++;//how many times did we get no IRQ
          textcolor(RED);
          cprintf("%-9s", "NoIRQ");
        }else{
          textcolor(GREEN);
          cprintf("%-9s", "IRQ");
        }
        idio16_isr_idioflag = 0;
      }//end for (8 bits)
      outportb(baddress+1,0x00); // Clear the interrupt.
      outportb(baddress+2,0x00); //disable interrupt
      outportb(baddress+1,0x00); // Clear the interrupt.
      sendEOI();                 //clear interrupt to allow next one
      restoreirq(IRQNumber);
    }// end while !kbhit()
    printf("\n");
    getch();
  }
  return flag;
} /* end interrupt_test */
