#include <dos.h>
#include <conio.h>
#include <stdio.h>
#include <graphics.h>
#include <stdlib.h>
#include "irq0-15.h"
#include <ctype.h>
#include "16550.h"
//#include "shareirq.h"

unsigned baddr2;
int isr_comflag;

//#pragma argsused
//int _16450DualTest(unsigned BaseA int df)
//{
//  int i=0;
//  char port[] = {'A','B','C','D'};
//  unsigned char ch,ch1,ch2,ch3;
//  unsigned flag = 0;
//  for (i = 0; i < maxport; i++){
//    outportb(Base[i] + LINE_CTRL,0x80); /* Set DLAB high for Divisor latch enable */
 //   outportb(Base[i] + L_DIVISOR,12);   /* Load lower latch with 12 */
 //   outportb(Base[i] + U_DIVISOR,0x00);    /* Load upper latch with 0  */
 //   outportb(Base[i] + LINE_CTRL,0x03);
 //   outportb(Base[i] + MODEM_CTRL,0x0D);   // place in receive
 //   outportb(Base[i] + INT_ENB, 0x00);
 //   outportb(Base[i] + INT_ID, 0x00);

  /* these inputs are to just insure that both the receiver buffer register
   and receiver shift register are clear. */

//  inportb(Base[i] + RECEIVE);   /* clear rx registers */
//  delay(50);
//  inportb(Base[i] + RECEIVE);   /* clear rx registers */
//  }//end initialization

//    outportb(Base[0] + MODEM_CTRL,0x0F); // set transmit

//    for (ch = 'A';ch <= 'F';ch++)
//    {
//      while (!(inportb(Base[0] + LINE_STAT) & 0x20));  // wait until ready to send
//      outportb(Base[0] + TRANSMIT, ch);
//      delay(50);
//      while (!(inportb(Base[0] + LINE_STAT) & 0x40));  // wait until sent
//      ch1 = inportb(Base[1] + RECEIVE);   /* read the input char */
//      delay(50);
//      ch2 = inportb(Base[2] + RECEIVE);
//      delay(50);
//      ch3 = inportb(Base[3] + RECEIVE);
//      printf("Out port %c: %c  In %c: %c   In %c: %c  In %c: %c\n",
//        port[i],ch,port[(i+1)%maxport],ch1,port[(i+2)%maxport],ch2,port[(i+3)%maxport],ch3);
//      if (ch1 != ch)
//        flag++;
//    }
//    outportb(Base[0] + MODEM_CTRL, 0x0D); /* Disable transmitter through RTS */
//  return flag;
//}

int _16450HalfDuplex(unsigned Base1, unsigned Base2,int df)
{

   char ch;
   char ch1;
   int flag = 0;
   int cnt = 0;
   int x = 13;
  outportb(Base1 + LINE_CTRL,0x80); /* Set DLAB high for Divisor latch enable */
  outportb(Base1 + L_DIVISOR,12);   /* Load lower latch with 12 */
  outportb(Base1 + U_DIVISOR,0x00);    /* Load upper latch with 0  */
  outportb(Base1 + LINE_CTRL,0x03);
  outportb(Base1 + MODEM_CTRL,0x0D);   // place in receive
  outportb(Base1 + INT_ENB, 0x00);
  outportb(Base1 + INT_ID, 0x00);
  outportb(Base2 + LINE_CTRL,0x80); /* Set DLAB high for Divisor latch enable */
  outportb(Base2 + L_DIVISOR,12);   /* Load lower latch with 12 */
  outportb(Base2 + U_DIVISOR,0x00);    /* Load upper latch with 0  */
  outportb(Base2 + LINE_CTRL,0x03);
  outportb(Base2 + MODEM_CTRL,0x0D);   // place in receive
  outportb(Base2 + INT_ENB, 0x00);
  outportb(Base2 + INT_ID, 0x00);
  if(df)
  {
    textcolor(LIGHTGRAY);
    cprintf("--Install the dual 485 wrap plug.\n\r");
    cprintf("--Jumpers??????\n\r");
    cprintf("Sent         A B C D E F G H I J K L M N O P Q R S T U V W X Y Z\n\r");
    cprintf("Received\n\r");
    gotoxy((wherex()+x),(wherey()-1));
  }
  outportb(Base1 + MODEM_CTRL,0x0F); //set transmit mode
  for (ch = 'A';ch <= 'Z';ch++)
  {
     delay(30);
     while (!(inportb(Base1 + LINE_STAT) & 0x20));  // wait until ready to send
     outportb(Base1 + TRANSMIT, ch);
     delay(30);
     while (!(inportb(Base1 + LINE_STAT) & 0x40) && cnt < 10000) cnt ++;  // wait until data is ready
     ch1 = inportb(Base2);   // read the input char
     if(df)
     {
       textcolor((ch==ch1)?GREEN:RED);
       cprintf("%c ",ch1);
     }
     if (ch1 != ch)
       flag ++;
   }
  if(df)
    printf("\n");
  outportb(Base1 + MODEM_CTRL,0x0D); //set receive mode
  return flag;

}

int _16450WDGFullDuplex(unsigned Base1, int df)
{

   char ch;
   char ch1;
   int flag = 0;
   int cnt = 0;
   int x = 13;
  outportb(Base1 + LINE_CTRL,0x80); /* Set DLAB high for Divisor latch enable */
  outportb(Base1 + L_DIVISOR,4);   /* Load lower latch with 12 */
  outportb(Base1 + U_DIVISOR,0x00);    /* Load upper latch with 0  */
  outportb(Base1 + LINE_CTRL,0x03);
  outportb(Base1 + MODEM_CTRL,0x0D);   // place in receive
  outportb(Base1 + INT_ENB, 0x00);
  outportb(Base1 + INT_ID, 0x00);
  if(df)
  {
    textcolor(LIGHTGRAY);
    cprintf("--Install the single WDG-2S 422 wrap plug on each port.\n\r");
    cprintf("--Install the appropriate jumpers.??????????\n\r");//what are the jumpers
    cprintf("Sent         A B C D E F G H I J K L M N O P Q R S T U V W X Y Z\n\r");
    cprintf("Received\n\r");
    gotoxy((wherex()+x),(wherey()-1));
  }
  outportb(Base1 + MODEM_CTRL,0x0C);
  for (ch = 'A';ch <= 'Z';ch++)
  {
     delay(50);
     while (!(inportb(Base1 + LINE_STAT) & 0x20));  // wait until ready to send
     outportb(Base1 + TRANSMIT, ch);
     delay(50);
     while (!(inportb(Base1 + LINE_STAT) & 0x01) && cnt < 10000) cnt ++;  // wait until data is ready
     ch1 = ((inportb(Base1 + RECEIVE) & 0X7F));   // read the input char
     if(df)
     {
       textcolor((ch==ch1)?GREEN:RED);
       cprintf("%c ",ch1);
     }
     if (ch1 != ch)
       flag ++;
   }
  if(df)
    printf("\n");
  return flag;

}


int _16450FullDuplex(unsigned Base1, int df)
{

   char ch;
   char ch1;
   int flag = 0;
   int cnt = 0;
   int x = 13;
  outportb(Base1 + LINE_CTRL,0x80); /* Set DLAB high for Divisor latch enable */
  outportb(Base1 + L_DIVISOR,4);   /* Load lower latch with 12 */
  outportb(Base1 + U_DIVISOR,0x00);    /* Load upper latch with 0  */
  outportb(Base1 + LINE_CTRL,0x03);
  outportb(Base1 + MODEM_CTRL,0x0D);   // place in receive
  outportb(Base1 + INT_ENB, 0x00);
  outportb(Base1 + INT_ID, 0x00);
  if(df)
  {
    textcolor(LIGHTGRAY);
    cprintf("--Install the single 422 wrap plug for the 422 test and the single\n\r");
    cprintf("232 wrap plug for the 232 test.\n\r");
    cprintf("--Install the appropriate 232 and 422 jumpers.\n\r");
    cprintf("Sent         A B C D E F G H I J K L M N O P Q R S T U V W X Y Z\n\r");
    cprintf("Received\n\r");
    gotoxy((wherex()+x),(wherey()-1));
  }
  outportb(Base1 + MODEM_CTRL,0x0C);
  for (ch = 'A';ch <= 'Z';ch++)
  {
     delay(50);
     while (!(inportb(Base1 + LINE_STAT) & 0x20));  // wait until ready to send
     outportb(Base1 + TRANSMIT, ch);
     delay(50);
     while (!(inportb(Base1 + LINE_STAT) & 0x01) && cnt < 10000) cnt ++;  // wait until data is ready
     ch1 = (inportb(Base1 + RECEIVE));// & 0X7F));   // read the input char
     if(df)
     {
       textcolor((ch==ch1)?GREEN:RED);
       cprintf("%c ",ch1);
     }
     if (ch1 != ch)
       flag ++;
   }
  if(df)
    printf("\n");
  return flag;

}

int _16450COM1SFullDuplex(unsigned Base1, int df)
{

   char ch;
   char ch1;
   int flag = 0;
   int cnt = 0;
   int x = 13;
  outportb(Base1 + LINE_CTRL,0x80); /* Set DLAB high for Divisor latch enable */
  outportb(Base1 + L_DIVISOR,4);   /* Load lower latch with 12 */
  outportb(Base1 + U_DIVISOR,0x00);    /* Load upper latch with 0  */
  outportb(Base1 + LINE_CTRL,0x03);
  outportb(Base1 + MODEM_CTRL,0x0D);   // place in receive
  outportb(Base1 + INT_ENB, 0x00);
  outportb(Base1 + INT_ID, 0x00);
  if(df)
  {
    textcolor(LIGHTGRAY);
    cprintf("--Install the single 422 wrap plug and the 422 jumper.\n\r");
    cprintf("Sent         A B C D E F G H I J K L M N O P Q R S T U V W X Y Z\n\r");
    cprintf("Received\n\r");
    gotoxy((wherex()+x),(wherey()-1));
  }
  outportb(Base1 + MODEM_CTRL,0x0C);
  for (ch = 'A';ch <= 'Z';ch++)
  {
     delay(50);
     while (!(inportb(Base1 + LINE_STAT) & 0x20));  // wait until ready to send
     outportb(Base1 + TRANSMIT, ch);
     delay(50);
     while (!(inportb(Base1 + LINE_STAT) & 0x01) && cnt < 10000) cnt ++;  // wait until data is ready
     ch1 = ((inportb(Base1 + RECEIVE) & 0X7F));   // read the input char
     if(df)
     {
       textcolor((ch==ch1)?GREEN:RED);
       cprintf("%c ",ch1);
     }
     if (ch1 != ch)
       flag ++;
   }
  if(df)
    printf("\n");
  return flag;

}

void far interrupt setcomflag(...)
{
  isr_comflag=inportb(baddr2+0);       //read character
  outportb(baddr2+INT_ENB,0x00);       //disable irq source
  sendEOI();                           //send End-of-IRQ to PIC
}   // end setflag

int _16450IRQTest(unsigned address, unsigned IRQ, int df)
{
  int result2 = 0;
  baddr2=address;
  isr_comflag=0;
 // inportb(0xfca4);
  outportb(address+INT_ENB,0x00);//       //disable irq source
  inportb(address + 0);          //
  sendEOI();

  initirq(IRQ,setcomflag);

  outportb(address + LINE_CTRL,0x80);  // Set DLAB high for Divisor latch enable
  outportb(address + L_DIVISOR,4);    // Load lower latch with 12
  outportb(address + U_DIVISOR,0x0);   // Load upper latch with 0
  outportb(address + LINE_CTRL,0x03);  // 8 bit data width, etc.
  outportb(address + INT_ENB, 0x00);   // disable all IRQ sources
  outportb(address + INT_ID, 0x00);    // disable TxRx FIFO
  outportb(address + MODEM_CTRL,0x08); // enable IRQs via out2
  inportb(address + 0);                  // clear receive irqs.
  outportb(address + INT_ENB,0x01);       //enable receive interrupt source
  outportb(address + 0,'U');             //generate IRQ
  delay(10);
  outportb(address + INT_ENB,0x00);//disable receive interrupt source
  restoreirq(IRQ);
  if(df)
  {
    textcolor(LIGHTGRAY);
   // cprintf("Install the AUTO and ECHO jumpers for each port.\n\r");
    cprintf("Press any key to exit IRQ debug.\n\r");
    delay(200);
    do
    {
      delay(100);
      isr_comflag=0;
      sendEOI();
      initirq(IRQ,setcomflag);
      inportb(address + 0);                  // clear receive irqs.
      outportb(address + INT_ENB,0x01);       //enable receive interrupt source
      outportb(address + 0,'U');             //generate IRQ
      delay(10);
      outportb(address + INT_ENB,0x00);//disable receive interrupt source
      restoreirq(IRQ);
      result2 = isr_comflag;
      textcolor(result2?GREEN:RED);
      cprintf("%s",result2?"IRQ    ":"NoIRQ    ");
      if((80 - wherex()) < 10)
        gotoxy(1,wherey());
      result2 = 0;
    }while(!kbhit()&&df);
  getch();
  printf("\n");
  }

  return (isr_comflag!='U');//return zero if U was successfully received in ISR
}

int _16450COM1SIRQTest(unsigned address, unsigned IRQ, int df)
{
  int result2 = 0;
  baddr2=address;
  isr_comflag=0;

  initirq(IRQ,setcomflag);
  outportb(address + LINE_CTRL,0x80);  // Set DLAB high for Divisor latch enable
  outportb(address + L_DIVISOR,4);    // Load lower latch with 12
  outportb(address + U_DIVISOR,0x0);   // Load upper latch with 0
  outportb(address + LINE_CTRL,0x03);  // 8 bit data width, etc.
  outportb(address + INT_ENB, 0x00);   // disable all IRQ sources
  outportb(address + INT_ID, 0x00);    // disable TxRx FIFO
  outportb(address + MODEM_CTRL,0x08); // enable IRQs via out2
  inportb(address + 0);                  // clear receive irqs.
  outportb(address + INT_ENB,0x01);       //enable receive interrupt source
  outportb(address + 0,'U');             //generate IRQ
  delay(10);
  restoreirq(IRQ);
  if(df)
  {
    textcolor(LIGHTGRAY);
    cprintf("Install the TERMIN jumper and 422 wrap plug.\n\r");
    cprintf("Press any key to exit IRQ debug.\n\r");
    delay(200);
    do
    {
      delay(100);
      isr_comflag=0;
      sendEOI();
      initirq(IRQ,setcomflag);
      inportb(address + 0);                  // clear receive irqs.
      outportb(address + INT_ENB,0x01);       //enable receive interrupt source
      outportb(address + 0,'U');             //generate IRQ
      delay(10);
      outportb(address + INT_ENB,0x00);//disable receive interrupt source
      restoreirq(IRQ);
      result2 = isr_comflag;
      textcolor(result2?GREEN:RED);
      cprintf("%s",result2?"IRQ    ":"NoIRQ    ");
      if((80 - wherex()) < 10)
        gotoxy(1,wherey());
      result2 = 0;
    }while(!kbhit()&&df);
  getch();
  printf("\n");
  }
  return (isr_comflag!='U');//return zero if U was successfully received in ISR
}

#pragma argsused
int _16450NotIRQTest(unsigned address, unsigned IRQ, int df)
{// char ch=0;
  baddr2=address;
  isr_comflag=0;

  initirq(IRQ,setcomflag);
  outportb(address + LINE_CTRL,0x80);  // Set DLAB high for Divisor latch enable
  outportb(address + L_DIVISOR,1);    // Load lower latch with 12
  outportb(address + U_DIVISOR,0x0);   // Load upper latch with 0
  outportb(address + LINE_CTRL,0x03);  // 8 bit data width, etc.
  outportb(address + INT_ENB, 0x00);   // disable all IRQ sources
  outportb(address + INT_ID, 0x00);    // disable TxRx FIFO
  outportb(address + MODEM_CTRL,0x08); // enable IRQs via out2
  inportb(address+0);                  // clear receive irqs.
  outportb(address +INT_ENB,0x01);       //enable receive interrupt source
  //outportb(address+0,'U');            //don't generate IRQ
  delay(10);
  outportb(address + INT_ENB,0x00);//disable receive interrupt source
  restoreirq(IRQ);
  return (isr_comflag=='U');//return zero if U was not successfully received in ISR
}

