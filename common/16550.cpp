#include "display.h"
#include "io.h"
#include "data_types.h"
#include "common_objects.h"
#include "irq.h"
#include "iiro8.h"
#include "16550.h"



unsigned baddr2;
int isr_comflag;


int _16450HalfDuplex(unsigned Base1, unsigned Base2,int df)
{

  char ch;
  char ch1;
  int flag = 0;
  int cnt = 0;
  int x = 13;
  OUTPORTB(Base1 + LINE_CTRL,0x80); /* Set DLAB high for Divisor latch enable */
  OUTPORTB(Base1 + L_DIVISOR,12);   /* Load lower latch with 12 */
  OUTPORTB(Base1 + U_DIVISOR,0x00);    /* Load upper latch with 0  */
  OUTPORTB(Base1 + LINE_CTRL,0x03);
  OUTPORTB(Base1 + MODEM_CTRL,0x0D);   // place in receive
  OUTPORTB(Base1 + INT_ENB, 0x00);
  OUTPORTB(Base1 + INT_ID, 0x00);
  OUTPORTB(Base2 + LINE_CTRL,0x80); /* Set DLAB high for Divisor latch enable */
  OUTPORTB(Base2 + L_DIVISOR,12);   /* Load lower latch with 12 */
  OUTPORTB(Base2 + U_DIVISOR,0x00);    /* Load upper latch with 0  */
  OUTPORTB(Base2 + LINE_CTRL,0x03);
  OUTPORTB(Base2 + MODEM_CTRL,0x0D);   // place in receive
  OUTPORTB(Base2 + INT_ENB, 0x00);
  OUTPORTB(Base2 + INT_ID, 0x00);
  if (df)
    {
      TEXT_COLOR(LIGHTGRAY);
      CPRINTF("--Install the dual 485 wrap plug.\n\r");
      CPRINTF("--Jumpers??????\n\r");
      CPRINTF("Sent         A B C D E F G H I J K L M N O P Q R S T U V W X Y Z\n\r");
      CPRINTF("Received\n\r");
      GOTOXY((WHEREX()+x),(WHEREY()-1));
    }
  OUTPORTB(Base1 + MODEM_CTRL,0x0F); //set transmit mode
  for (ch = 'A';ch <= 'Z';ch++)
    {
      DELAY(30);
      while (!(INPORTB(Base1 + LINE_STAT) & 0x20));  // wait until ready to send
      OUTPORTB(Base1 + TRANSMIT, ch);
      DELAY(30);
      while (!(INPORTB(Base1 + LINE_STAT) & 0x40) && cnt < 10000) cnt ++;  // wait until data is ready
      ch1 = INPORTB(Base2);   // read the input char
      if (df)
        {
          TEXT_COLOR((ch==ch1)?GREEN:RED);
          CPRINTF("%c ",ch1);
        }
      if (ch1 != ch)
        flag ++;
    }
  if (df)
    CPRINTF("\n");
  OUTPORTB(Base1 + MODEM_CTRL,0x0D); //set receive mode
  return flag;

}

int _16450WDGFullDuplex(unsigned Base1, int df)
{

  char ch;
  char ch1;
  int flag = 0;
  int cnt = 0;
  int x = 13;
  OUTPORTB(Base1 + LINE_CTRL,0x80); /* Set DLAB high for Divisor latch enable */
  OUTPORTB(Base1 + L_DIVISOR,4);   /* Load lower latch with 12 */
  OUTPORTB(Base1 + U_DIVISOR,0x00);    /* Load upper latch with 0  */
  OUTPORTB(Base1 + LINE_CTRL,0x03);
  OUTPORTB(Base1 + MODEM_CTRL,0x0D);   // place in receive
  OUTPORTB(Base1 + INT_ENB, 0x00);
  OUTPORTB(Base1 + INT_ID, 0x00);
  if (df)
    {
      TEXT_COLOR(LIGHTGRAY);
      CPRINTF("--Install the single WDG-2S 422 wrap plug on each port.\n\r");
      CPRINTF("--Install the appropriate jumpers.??????????\n\r");//what are the jumpers
      CPRINTF("Sent         A B C D E F G H I J K L M N O P Q R S T U V W X Y Z\n\r");
      CPRINTF("Received\n\r");
      GOTOXY((WHEREX()+x),(WHEREY()-1));
    }
  OUTPORTB(Base1 + MODEM_CTRL,0x0C);
  for (ch = 'A';ch <= 'Z';ch++)
    {
      DELAY(50);
      while (!(INPORTB(Base1 + LINE_STAT) & 0x20));  // wait until ready to send
      OUTPORTB(Base1 + TRANSMIT, ch);
      DELAY(50);
      while (!(INPORTB(Base1 + LINE_STAT) & 0x01) && cnt < 10000) cnt ++;  // wait until data is ready
      ch1 = ((INPORTB(Base1 + RECEIVE) & 0X7F));   // read the input char
      if (df)
        {
          TEXT_COLOR((ch==ch1)?GREEN:RED);
          CPRINTF("%c ",ch1);
        }
      if (ch1 != ch)
        flag ++;
    }
  if (df)
    CPRINTF("\n");
  return flag;

}


int _16450FullDuplex(unsigned Base1, int df)
{

  char ch;
  char ch1;
  int flag = 0;
  int cnt = 0;
  int x = 13;
  OUTPORTB(Base1 + LINE_CTRL,0x80); /* Set DLAB high for Divisor latch enable */
  OUTPORTB(Base1 + L_DIVISOR,4);   /* Load lower latch with 12 */
  OUTPORTB(Base1 + U_DIVISOR,0x00);    /* Load upper latch with 0  */
  OUTPORTB(Base1 + LINE_CTRL,0x03);
  OUTPORTB(Base1 + MODEM_CTRL,0x0D);   // place in receive
  OUTPORTB(Base1 + INT_ENB, 0x00);
  OUTPORTB(Base1 + INT_ID, 0x00);
  if (df)
    {
      TEXT_COLOR(LIGHTGRAY);
      CPRINTF("--Install the single 422 wrap plug for the 422 test and the single\n\r");
      CPRINTF("232 wrap plug for the 232 test.\n\r");
      CPRINTF("--Install the appropriate 232 and 422 jumpers.\n\r");
      CPRINTF("Sent         A B C D E F G H I J K L M N O P Q R S T U V W X Y Z\n\r");
      CPRINTF("Received\n\r");
      GOTOXY((WHEREX()+x),(WHEREY()-1));
    }
  OUTPORTB(Base1 + MODEM_CTRL,0x0C);
  for (ch = 'A';ch <= 'Z';ch++)
    {
      DELAY(50);
      while (!(INPORTB(Base1 + LINE_STAT) & 0x20));  // wait until ready to send
      OUTPORTB(Base1 + TRANSMIT, ch);
      DELAY(50);
      while (!(INPORTB(Base1 + LINE_STAT) & 0x01) && cnt < 10000) cnt ++;  // wait until data is ready
      ch1 = (INPORTB(Base1 + RECEIVE));// & 0X7F));   // read the input char
      if (df)
        {
          TEXT_COLOR((ch==ch1)?GREEN:RED);
          CPRINTF("%c ",ch1);
        }
      if (ch1 != ch)
        flag ++;
    }
  if (df)
    CPRINTF("\n");
  return flag;

}

int _16450COM1SFullDuplex(unsigned Base1, int df)
{

  char ch;
  char ch1;
  int flag = 0;
  int cnt = 0;
  int x = 13;
  OUTPORTB(Base1 + LINE_CTRL,0x80); /* Set DLAB high for Divisor latch enable */
  OUTPORTB(Base1 + L_DIVISOR,4);   /* Load lower latch with 12 */
  OUTPORTB(Base1 + U_DIVISOR,0x00);    /* Load upper latch with 0  */
  OUTPORTB(Base1 + LINE_CTRL,0x03);
  OUTPORTB(Base1 + MODEM_CTRL,0x0D);   // place in receive
  OUTPORTB(Base1 + INT_ENB, 0x00);
  OUTPORTB(Base1 + INT_ID, 0x00);
  if (df)
    {
      TEXT_COLOR(LIGHTGRAY);
      CPRINTF("--Install the single 422 wrap plug and the 422 jumper.\n\r");
      CPRINTF("Sent         A B C D E F G H I J K L M N O P Q R S T U V W X Y Z\n\r");
      CPRINTF("Received\n\r");
      GOTOXY((WHEREX()+x),(WHEREY()-1));
    }
  OUTPORTB(Base1 + MODEM_CTRL,0x0C);
  for (ch = 'A';ch <= 'Z';ch++)
    {
      DELAY(50);
      while (!(INPORTB(Base1 + LINE_STAT) & 0x20));  // wait until ready to send
      OUTPORTB(Base1 + TRANSMIT, ch);
      DELAY(50);
      while (!(INPORTB(Base1 + LINE_STAT) & 0x01) && cnt < 10000) cnt ++;  // wait until data is ready
      ch1 = ((INPORTB(Base1 + RECEIVE) & 0X7F));   // read the input char
      if (df)
        {
          TEXT_COLOR((ch==ch1)?GREEN:RED);
          CPRINTF("%c ",ch1);
        }
      if (ch1 != ch)
        flag ++;
    }
  if (df)
    CPRINTF("\n");
  return flag;

}

void far interrupt setcomflag(...)
{
  isr_comflag=INPORTB(baddr2+0);       //read character
  OUTPORTB(baddr2+INT_ENB,0x00);       //disable irq source
  sendEOI();                           //send End-of-IRQ to PIC
}   // end setflag

int _16450IRQTest(unsigned address, unsigned IRQ, int df)
{
  int result2 = 0;
  baddr2=address;
  isr_comflag=0;
  // INPORTB(0xfca4);
  OUTPORTB(address+INT_ENB,0x00);//       //disable irq source
  INPORTB(address + 0);          //
  sendEOI();

  // initirq(IRQ,setcomflag);

  OUTPORTB(address + LINE_CTRL,0x80);  // Set DLAB high for Divisor latch enable
  OUTPORTB(address + L_DIVISOR,4);    // Load lower latch with 12
  OUTPORTB(address + U_DIVISOR,0x0);   // Load upper latch with 0
  OUTPORTB(address + LINE_CTRL,0x03);  // 8 bit data width, etc.
  OUTPORTB(address + INT_ENB, 0x00);   // disable all IRQ sources
  OUTPORTB(address + INT_ID, 0x00);    // disable TxRx FIFO
  OUTPORTB(address + MODEM_CTRL,0x08); // enable IRQs via out2
  INPORTB(address + 0);                  // clear receive irqs.
  OUTPORTB(address + INT_ENB,0x01);       //enable receive interrupt source
  OUTPORTB(address + 0,'U');             //generate IRQ
  DELAY(10);
  OUTPORTB(address + INT_ENB,0x00);//disable receive interrupt source
  restoreirq(IRQ);
  if (df)
    {
      TEXT_COLOR(LIGHTGRAY);
      // CPRINTF("Install the AUTO and ECHO jumpers for each port.\n\r");
      CPRINTF("Press any key to exit IRQ debug.\n\r");
      DELAY(200);
      do
        {
          DELAY(100);
          isr_comflag=0;
          sendEOI();
          // initirq(IRQ,setcomflag);
          INPORTB(address + 0);                  // clear receive irqs.
          OUTPORTB(address + INT_ENB,0x01);       //enable receive interrupt source
          OUTPORTB(address + 0,'U');             //generate IRQ
          DELAY(10);
          OUTPORTB(address + INT_ENB,0x00);//disable receive interrupt source
          restoreirq(IRQ);
          result2 = isr_comflag;
          TEXT_COLOR(result2?GREEN:RED);
          CPRINTF("%s",result2?"IRQ    ":"NoIRQ    ");
          if ((80 - WHEREX()) < 10)
            GOTOXY(1,WHEREY());
          result2 = 0;
        }while(!KBHIT()&&df);
      GETCH();
      CPRINTF("\n");
    }

  return (isr_comflag!='U');//return zero if U was successfully received in ISR
}

int _16450COM1SIRQTest(unsigned address, unsigned IRQ, int df)
{
  int result2 = 0;
  baddr2=address;
  isr_comflag=0;

  // initirq(IRQ,setcomflag);
  OUTPORTB(address + LINE_CTRL,0x80);  // Set DLAB high for Divisor latch enable
  OUTPORTB(address + L_DIVISOR,4);    // Load lower latch with 12
  OUTPORTB(address + U_DIVISOR,0x0);   // Load upper latch with 0
  OUTPORTB(address + LINE_CTRL,0x03);  // 8 bit data width, etc.
  OUTPORTB(address + INT_ENB, 0x00);   // disable all IRQ sources
  OUTPORTB(address + INT_ID, 0x00);    // disable TxRx FIFO
  OUTPORTB(address + MODEM_CTRL,0x08); // enable IRQs via out2
  INPORTB(address + 0);                  // clear receive irqs.
  OUTPORTB(address + INT_ENB,0x01);       //enable receive interrupt source
  OUTPORTB(address + 0,'U');             //generate IRQ
  DELAY(10);
  restoreirq(IRQ);
  if (df)
    {
      TEXT_COLOR(LIGHTGRAY);
      CPRINTF("Install the TERMIN jumper and 422 wrap plug.\n\r");
      CPRINTF("Press any key to exit IRQ debug.\n\r");
      DELAY(200);
      do
        {
          DELAY(100);
          isr_comflag=0;
          sendEOI();
          // initirq(IRQ,setcomflag);
          INPORTB(address + 0);                  // clear receive irqs.
          OUTPORTB(address + INT_ENB,0x01);       //enable receive interrupt source
          OUTPORTB(address + 0,'U');             //generate IRQ
          DELAY(10);
          OUTPORTB(address + INT_ENB,0x00);//disable receive interrupt source
          restoreirq(IRQ);
          result2 = isr_comflag;
          TEXT_COLOR(result2?GREEN:RED);
          CPRINTF("%s",result2?"IRQ    ":"NoIRQ    ");
          if ((80 - WHEREX()) < 10)
            GOTOXY(1,WHEREY());
          result2 = 0;
        } while ( !KBHIT() &&df );
      GETCH();
      CPRINTF("\n");
    }
  return (isr_comflag!='U');//return zero if U was successfully received in ISR
}


int _16450NotIRQTest(unsigned address, unsigned IRQ, int df)
{// char ch=0;
  baddr2=address;
  isr_comflag=0;

  // initirq(IRQ,setcomflag);
  OUTPORTB(address + LINE_CTRL,0x80);  // Set DLAB high for Divisor latch enable
  OUTPORTB(address + L_DIVISOR,1);    // Load lower latch with 12
  OUTPORTB(address + U_DIVISOR,0x0);   // Load upper latch with 0
  OUTPORTB(address + LINE_CTRL,0x03);  // 8 bit data width, etc.
  OUTPORTB(address + INT_ENB, 0x00);   // disable all IRQ sources
  OUTPORTB(address + INT_ID, 0x00);    // disable TxRx FIFO
  OUTPORTB(address + MODEM_CTRL,0x08); // enable IRQs via out2
  INPORTB(address+0);                  // clear receive irqs.
  OUTPORTB(address +INT_ENB,0x01);       //enable receive interrupt source
  //OUTPORTB(address+0,'U');            //don't generate IRQ
  DELAY(10);
  OUTPORTB(address + INT_ENB,0x00);//disable receive interrupt source
  restoreirq(IRQ);
  return (isr_comflag=='U');//return zero if U was not successfully received in ISR
}

