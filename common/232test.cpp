#include "display.h"
#include "io.h"
#include "data_types.h"
#include "common_objects.h"
#include "irq.h"
#include "16550.h"



void DTRTSTest(unsigned short *base, int index, unsigned char OutByte, unsigned char InNybble, const char* TestName)
{

  unsigned Result = 0;
  unsigned char ReadByte, OldLCR;
    OUTPORTB(base[index] + 4, OutByte);
    DELAY(1);
    ReadByte = INPORTB(base[index] + 6);
    if ( (ReadByte & 0xF0) != InNybble )
      Result |= 1 << index
    ;

    OldLCR = INPORTB(base[index] + 3);
    OUTPORTB(base[index] + 3, 0xBF);
    OUTPORTB(base[index] + 2, 0x40);
    OUTPORTB(base[index] + 3, OldLCR);


  if ( Result == 0 )
  {
    TEXT_COLOR(GREEN);
    CPRINTF("      %s Test       %43s\n\r", TestName, "PASSED");
  }
  else
  {
    TEXT_COLOR(RED);
    CPRINTF("      %s Test\n\r", TestName);
    if ( Result & (1 << index) )
      CPRINTF("               COM %c %43s\n\r", 'A' + index, "FAILED")
    ;
  }
}

int _232Test(unsigned Base1)
{
   char ch;
   char ch1;
   int flag = 0;
   int cnt = 0;
   OUTPORTB(Base1 + MODEM_CTRL,0x0C);
   for (ch = 'A';ch <= 'Z';ch++)
     {
       while (!(INPORTB(Base1 + LINE_STAT) & 0x20));  // wait until ready to send
       OUTPORTB(Base1 + TRANSMIT, ch);
       DELAY(10);
       while (!(INPORTB(Base1 + LINE_STAT) & 0x01) && cnt < 10000) cnt ++;  // wait until data is ready
       ch1 = ((INPORTB(Base1 + RECEIVE) & 0X7F));   // read the input char
       if (ch1 != ch)
     flag ++;
     }
     if (flag == 0)
       return 1;
     else
       return 0;
}
