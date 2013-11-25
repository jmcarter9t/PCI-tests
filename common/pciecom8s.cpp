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
dword vendev=0L;
byte bn=0;                           //bus_number
byte df = 0;                         //device_and_function
byte regnum;
word Index;
byte h;//temp values for debug
int foundcount=0;
word basea,baseb,basec,based,basee,irqnum;



/*****************************************************************************
 *  FUNCTION: relay_write_test -- local routine                              *
 *****************************************************************************/

void eCOM(char *name, byte numport, unsigned short *base, char *prot)
{
    char port [8] = {'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H'};
    const char *p[4] ={"RS-232", "RS-422", "RS-485", "!ERROR!"};
    unsigned long PericomControlValuePerProtocolAllPorts[] = {0x00000000L, 0x11111111L, 0xFFFFFFFFL, 0};

    CPRINTF("%40s\n\r",name);

    for (byte protocol = 0; protocol <= 2; protocol++)
    {
        if (prot[protocol] != S_IGNORE)
        {
            CPRINTF("Press any key to begin %s test -- Press ESC to skip this step\n\r",p[prot[protocol]]);
            if (getch() == 27) continue;

            write_configuration_dword(GLOBALS.bn,GLOBALS.df,regnum = 0xB4, PericomControlValuePerProtocolAllPorts[prot[protocol]]);

            for (byte Index = 0; Index < numport; Index++)
            {
                TEXT_COLOR(result?RED:GREEN);
                CPRINTF("      COM %c:%04X  IRQ:%hu%40s\n\r", port[Index], base[Index], irqnum, "FOUND");
                switch(prot[protocol])
                {
                case S_232:
                  DTRTSTest(base, Index, 0x01, 0xA0, "DTR");
                  DTRTSTest(base, Index, 0x0A, 0x50, "RTS");
                case S_422:
                     result= _16450IRQTest( base[Index], irqnum, DebugFlag);
                     TEXT_COLOR(result?RED:GREEN);
                     CPRINTF("      IRQ  COM %c %47s\n\r", port[Index],result?"FAILED":"PASSED");

                     result = _16450NotIRQTest(base[Index],irqnum,DebugFlag);
                     TEXT_COLOR(result?RED:GREEN);
                     CPRINTF("      Not IRQ  COM %c %43s\n\r", port[Index],result?"FAILED":"PASSED");

                     result = _16450FullDuplex(base[Index],DebugFlag);
                     TEXT_COLOR(result?RED:GREEN);
                     CPRINTF("      COM Test COM %c%43s\n\r",port[Index],result?"FAILED":"PASSED");
                     break;
                case S_485:
                     result = _16450ShareIRQDebug(base[0],base[1],base[2],base[3],base[4],base[5],base[6],base[7],irqnum,numport,Index,DebugFlag);
                     TEXT_COLOR( result ? RED : GREEN );
                     CPRINTF("      COM %c Output Test %40s\n\r",port[Index], result?"FAILED":"PASSED");

                     break;

                }
            }
        }
    }
}

 
void PCIeCOM8S(char *name, int numport, byte F1, byte F2, byte F3)
{
   int Index ;
   int i;
   byte prot[] = {F1,F2,F3};
   byte start = 0x10;  //location of BAR[0]
   unsigned short base [8];
   char port [8] = {'A','B','C','D','E','F','G','H'};
   CPRINTF("%40s\n\r",name);
   //maxport = 8;
   result = 0;
   result += read_configuration_word(GLOBALS.bn,GLOBALS.df,regnum = 0x3C,&irqnum);
   irqnum  &=  0xF;
   result += read_configuration_word(GLOBALS.bn,GLOBALS.df,regnum = start,&base[0]);
   base[0]  &=  0xFFFE;
   base[1]=base[0]+8;
   base[2]=base[0]+16;
   base[3]=base[0]+0x18;
   base[4]=base[0]+0x20;
   base[5]=base[0]+0x28;
   base[6]=base[0]+0x30;
   base[7]=base[0]+0x38;
   eCOM( name, numport, base, port );
}
