#include <dos.h>
#include "16550.h"
#include <conio.h>


void DTRTSTest(unsigned short *base, int index, unsigned char OutByte, unsigned char InNybble, char* TestName)
{

  unsigned Result = 0;
  unsigned char ReadByte, OldLCR;
    outportb(base[index] + 4, OutByte);
    delay(1);
    ReadByte = inportb(base[index] + 6);
    if ( (ReadByte & 0xF0) != InNybble )
      Result |= 1 << index
    ;

    OldLCR = inportb(base[index] + 3);
    outportb(base[index] + 3, 0xBF);
    outportb(base[index] + 2, 0x40);
    outportb(base[index] + 3, OldLCR);


  if ( Result == 0 )
  {
    textcolor(GREEN);
    cprintf("      %s Test       %43s\n\r", TestName, "PASSED");
  }
  else
  {
    textcolor(RED);
    cprintf("      %s Test\n\r", TestName);
    if ( Result & (1 << index) )
      cprintf("               COM %c %43s\n\r", 'A' + index, "FAILED")
    ;
  }
}

int _232Test(unsigned Base1)
{
   char ch;
   char ch1;
   int flag = 0;
   int cnt = 0;
   outportb(Base1 + MODEM_CTRL,0x0C);
   for (ch = 'A';ch <= 'Z';ch++)
     {
       while (!(inportb(Base1 + LINE_STAT) & 0x20));  // wait until ready to send
       outportb(Base1 + TRANSMIT, ch);
       delay(10);
       while (!(inportb(Base1 + LINE_STAT) & 0x01) && cnt < 10000) cnt ++;  // wait until data is ready
       ch1 = ((inportb(Base1 + RECEIVE) & 0X7F));   // read the input char
       if (ch1 != ch)
     flag ++;
     }
     if (flag == 0)
       return 1;
     else
       return 0;
}
