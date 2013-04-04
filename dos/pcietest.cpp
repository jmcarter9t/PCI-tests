#include <string.h>


#ifdef DOS
#include <ctype.h>
#include <dos.h>
#include <conio.h>
#include <graphics.h>
#elsif LINUX


#else

#endif

#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <stddef.h>


#include "8255test.h"
#include "16550.h"
#include "8254.h"
#include "pciutil.h"
#include "485Test.h"
#include "iiro8.h"
#include "iiro16.h"
#include "idio16.h"
#include "dac.h"
#include "ai.h"
#include "idiido.h"
#include "irq0-15.h"
#include "error_codes.h"

#pragma inline

int DebugFlag;
unsigned int Flag96 = 0;
int result = 0;
dword vendev=0L;
byte bn=0;                           //bus_number
byte df = 0;                         //device_and_function
byte regnum;
word index;
byte h;//temp values for debug
int foundcount=0;
word basea,baseb,basec,based,basee,irqnum;

typedef enum {PCIeDIO24DS,OTHER,COM1S, COM2S, COM, PericomCOM4S, PericomCOM8S, COM8S, DIO, DIO22, IIRO8, WDGCSM, WDG2S, DA, AI, IIRO16, IDIO16, PCIA1216A, IDI, IDO, eDIO} infotype;

typedef struct {
  unsigned   dev;        //deviceID
  infotype   type;
  char       cn[80];     //CardName
  char       n;          //Number of "ports"
  char       f1;
  char       f2;
  char       f3;
  char       f4;
  char       f5;
} tcardlist;//end struct

tcardlist list[]=
      {
        // cards based on PCIe-DIO-24DCS model
        { 0x0e56, PCIeDIO24DS,  "PCIe-DIO-24DCS w/counter(s)", 1, NO48,  YESCOS,  YESCOUNT, YESHD },
        { 0x0e55, PCIeDIO24DS,  "PCIe-DIO-24DC w/counter(s)" , 1, NO48,  NOCOS ,  YESCOUNT, YESHD },
        { 0x0e54, PCIeDIO24DS,  "PCIe-DIO-24DS"              , 1, NO48,  YESCOS,  NOCOUNT , YESHD },
        { 0x0C53, PCIeDIO24DS,  "PCIe-DIO-24D"               , 1, NO48,  NOCOS ,  NOCOUNT , YESHD },

        { 0x2EE0, PCIeDIO24DS, "PCIe-DIO-24S-CTR12"          , 1, NO48, YESCOS , YESCOUNT, YESHD },

        // cards based on PCIe-DIO-48S model
        { 0x0C61, eDIO       ,  "PCIe-DIO-48"                , 2, YES48, NOCOS , NOCOUNT, NOHD },
        { 0x0E61, eDIO       ,  "PCIe-DIO-48S"               , 2, NO48 , YESCOS, NOCOUNT, NOHD },
        { 0x0C52, eDIO       ,  "PCIe-DIO-24"                , 1, YES48, NOCOS , NOCOUNT, NOHD },
        { 0x0E53, eDIO       ,  "PCIe-DIO-24S"               , 1, NO48 , YESCOS, NOCOUNT, NOHD },

        // cards based on PCIe-COM-8SM model
        { 0x10E9, PericomCOM8S ,"PCIe-COM-8SM"               , 8, S_232, S_422   , S_485   , S_IGNORE },
        { 0x10D9, PericomCOM4S ,"PCIe-COM-4SM"               , 4, S_232, S_422   , S_485   , S_IGNORE },
        { 0x106A, PericomCOM8S ,"PCIe-COM422-8"              , 8, S_422, S_IGNORE, S_IGNORE, S_IGNORE },
        { 0x105A, PericomCOM4S ,"PCIe-COM422-4"              , 4, S_422, S_IGNORE, S_IGNORE, S_IGNORE },
        { 0x106B, PericomCOM8S ,"PCIe-COM485-8"              , 8, S_485, S_IGNORE, S_IGNORE, S_IGNORE },
        { 0x105B, PericomCOM4S ,"PCIe-COM485-4"              , 4, S_485, S_IGNORE, S_IGNORE, S_IGNORE },
        { 0x10A9, PericomCOM8S ,"PCIe-COM232-8"              , 8, S_232, S_IGNORE, S_IGNORE, S_IGNORE },
        { 0x1098, PericomCOM4S ,"PCIe-COM232-4"              , 4, S_232, S_IGNORE, S_IGNORE, S_IGNORE },

        // cards based on PCIe-COM-4SMRJ/DB model
        { 0x10DA, PericomCOM4S ,"PCIe-COM-4SMRJ/DB"          , 4, S_232, S_422   , S_485   , S_IGNORE },
        { 0x10D1, PericomCOM4S ,"PCIe-COM-2SMRJ/DB"          , 2, S_232, S_422   , S_485   , S_IGNORE },
        { 0x105C, PericomCOM4S ,"PCIe-COM-4SRJ/DB"           , 4, S_422, S_485   , S_IGNORE, S_IGNORE },
        { 0x1051, PericomCOM4S ,"PCIe-COM-2SRJ/DB"           , 2, S_422, S_485   , S_IGNORE, S_IGNORE },
        { 0x1099, PericomCOM4S ,"PCIe-COM232-4RJ/DB"         , 4, S_232, S_IGNORE, S_IGNORE, S_IGNORE },
        { 0x1091, PericomCOM4S ,"PCIe-COM232-2RJ/DB"         , 2, S_232, S_IGNORE, S_IGNORE, S_IGNORE },

        // cards based on PCIe-ICM-4SM model
        { 0x11D8, PericomCOM4S ,"ACCES PCIe-ICM-4SM"         , 4, S_232, S_422   , S_485   , S_IGNORE },
        { 0x115A, PericomCOM4S ,"ACCES PCIe-ICM-4S"          , 4, S_422, S_485   , S_IGNORE, S_IGNORE },
        { 0x1198, PericomCOM4S ,"ACCES PCIe-ICM232-4"        , 4, S_232, S_IGNORE, S_IGNORE, S_IGNORE },
        { 0x11D0, PericomCOM4S ,"ACCES PCIe-ICM-2SM"         , 2, S_232, S_422   , S_485   , S_IGNORE },
        { 0x1152, PericomCOM4S ,"ACCES PCIe-ICM-2S"          , 2, S_422, S_485   , S_IGNORE, S_IGNORE },
        { 0x1190, PericomCOM4S ,"ACCES PCIe-ICM232-2"        , 2, S_232, S_IGNORE, S_IGNORE, S_IGNORE },

        { 0x0f02, IIRO8        ,"PCIe-IIRO-8"                , 1, IGNORE,IGNORE,IGNORE,IGNORE},
        { 0x0f09, IIRO16       ,"PCIe-IIRO-16"               , 1, IGNORE,IGNORE,IGNORE,IGNORE},



        {0x0C50,DIO   ,"PCI-DIO-24H"      ,1,NO48,NOCOS, NOCOUNT,YESHD},
        {0x0C51,DIO   ,"PCI-DIO-24D"      ,1,NO48,NOCOS, NOCOUNT,YESHD},
        {0x0C60,DIO   ,"PCI-DIO-48"       ,2,YES48,NOCOS,NOCOUNT,NOHD},
        {0x0c68,DIO   ,"PCI-DIO-72"       ,3,NO48,NOCOS,NOCOUNT,NOHD},
        {0x0C69,DIO22 ,"P104-DIO-96"      ,4,IGNORE,IGNORE,IGNORE,IGNORE},
        {0x0c70,DIO   ,"PCI-DIO-96"       ,4,NO48,NOCOS,NOCOUNT,NOHD},
        {0x0c78,DIO   ,"PCI-DIO-120"      ,5,NO48,NOCOS,NOCOUNT,NOHD},
        {0x0E50,DIO   ,"PCI-DIO-24S"      ,1,NO48,YESCOS,NOCOUNT,YESHD},
        {0x0e51,DIO   ,"PCI-DIO-24H w/counter(s)",1,NO48,NOCOS,  YESCOUNT,YESHD},
        {0x0e52,DIO   ,"PCI-DIO-24D w/counter(s)",1,NO48,NOCOS,  YESCOUNT,YESHD},
        {0x0E60,DIO   ,"PCI-DIO-48S"      ,2,NO48,YESCOS,NOCOUNT,NOHD},
        {0x1058,COM   ,"PCI-COM422/4"     ,4,NO485,IGNORE,IGNORE,IGNORE},
        {0x1059,COM   ,"PCI-COM485/4"     ,4,YES485,IGNORE,IGNORE,IGNORE},
        {0x1068,COM8S ,"PCI-COM422/8"     ,8,NO485,IGNORE,IGNORE,IGNORE},
        {0x1069,COM8S ,"PCI-COM485/8"     ,8,YES485,IGNORE,IGNORE,IGNORE},
        {0x1088,COM2S ,"PCI-COM232/1"     ,1,NO485,IGNORE,IGNORE,IGNORE},
        {0x1090,COM2S ,"PCI-COM232/2"     ,2,NO485,IGNORE,IGNORE,IGNORE},
        {0x10c8,COM1S ,"PCI-COM1S (2Spcb)",1,NO232,IGNORE,IGNORE,IGNORE},
        {0x10c9,COM1S ,"PCI-COM-1S"       ,1,IGNORE,IGNORE,IGNORE,IGNORE},
        {0x10d0,COM2S ,"PCI-COM2S"        ,2,NO485,IGNORE,IGNORE,IGNORE},
        {0x10e8,COM8S ,"LPCI-COM-8SM(422/232)" ,8,NO485,IGNORE,IGNORE,IGNORE},
        {0x1250,WDG2S ,"PCI-WDG-2S"       ,2,IGNORE,IGNORE,IGNORE,IGNORE},
        {0x12d0,COM2S ,"PCI-WDG-IMPAC"    ,2,NO485,IGNORE,IGNORE,IGNORE},
        {0x2c50,DIO   ,"PCI-DIO-96CT"     ,4,NO48,NOCOS,YESCOUNT,NOHD},
        {0x2c58,DIO   ,"PCI-DIO-96C3"     ,4,NO48,NOCOS,YESCOUNT,NOHD},

        {0x0520,IDO   ,"PCI-IDO-48"       ,1,IGNORE,IGNORE,IGNORE,IGNORE},
        {0x0920,IDI   ,"PCI-IDI-48"       ,1,IGNORE,IGNORE,IGNORE,IGNORE},
        {0x0DC8,IDIO16,"PCI-IDIO-16"      ,1,IGNORE,IGNORE,IGNORE,IGNORE},
        {0x0f00,IIRO8 ,"PCI-IIRO-8"       ,1,IGNORE,IGNORE,IGNORE,IGNORE},
        {0x0f01,IIRO8 ,"LPCI-IIRO-8"      ,1,IGNORE,IGNORE,IGNORE,IGNORE},
        {0x0f08,IIRO16,"PCI-IIRO-16"      ,1,IGNORE,IGNORE,IGNORE,IGNORE},
        {0x1050,COM2S ,"PCI-422/485-2(422)",2,NO485,IGNORE,IGNORE,IGNORE},
        {0x1050,COM2S ,"PCI-422/485-2(485)",2,YES485,IGNORE,IGNORE,IGNORE},
        {0x1148,COM1S ,"PCI-ICM-1S"       ,1,NO485,IGNORE,IGNORE,IGNORE},
        {0x1150,COM2S ,"PCI-ICM-2S(485)"  ,2,YES485,IGNORE,IGNORE,IGNORE},
        {0x1150,COM2S ,"PCI-ICM-2S(422)"  ,2,NO485,IGNORE,IGNORE,IGNORE},
        {0x1158,COM   ,"PCI-ICM422/4"     ,4,NO485,IGNORE,IGNORE,IGNORE},
        {0x1159,COM   ,"PCI-ICM485/4"     ,4,YES485,IGNORE,IGNORE,IGNORE},
        {0x22C0,WDGCSM,"PCI-WDG-CSM"      ,1,IGNORE,IGNORE,IGNORE,IGNORE},
        {0x5eD0,OTHER ,"bPCI-DAC"         ,1,IGNORE,IGNORE,IGNORE,IGNORE},
        {0x6C90,DA    ,"PCI-DA12-2"       ,2,IGNORE,IGNORE,IGNORE,IGNORE},
        {0x6C98,DA    ,"PCI-DA12-4"       ,4,IGNORE,IGNORE,IGNORE,IGNORE},
        {0x6CA0,DA    ,"PCI-DA12-6"       ,6,IGNORE,IGNORE,IGNORE,IGNORE},
        {0x6CA8,DA    ,"PCI-DA12-8"       ,8,IGNORE,IGNORE,IGNORE,IGNORE},
        {0x6CA9,DA    ,"PCI-DA12-8V"      ,8,IGNORE,IGNORE,IGNORE,IGNORE},
        {0x6CB0,DA    ,"PCI-DA12-16"      ,16,IGNORE,IGNORE,IGNORE,IGNORE},
        {0x6CB1,DA    ,"PCI-DA12-16V"     ,16,IGNORE,IGNORE,IGNORE,IGNORE},
        {0xACA8,AI    ,"PCI-AI12-16"      ,16,NOFIFO,IGNORE,IGNORE,IGNORE},
        {0xACA9,AI    ,"PCI-AI12-16A"     ,16,YESFIFO,IGNORE,IGNORE,IGNORE},
        {0xECAA,PCIA1216A,"PCI-A12-16A"   ,16,YESFIFO,IGNORE,IGNORE,IGNORE},
      };

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
{ int rn;
  result = read_configuration_word(bn,df,rn = 0x18,&basea);
  result = read_configuration_word(bn,df,rn = 0x1C,&baseb);
  result = read_configuration_word(bn,df,rn = 0x20,&basec);
  result = read_configuration_word(bn,df,rn = 0x24,&based);
  result += read_configuration_word(bn,df,rn = 0x3C,&irqnum);
  basea&=0xFFFE;
  baseb&=0xFFFE;
  basec&=0xFFFE;
  based&=0xFFFE;
  if (!result){
    printf("A board has been located.\n");
    printf("Base Address A= %X\n",basea);
    printf("Base Address B= %X\n",baseb);
    printf("Base Address C= %X\n",basec);
    printf("Base Address D= %X\n",based);
    printf("IRQ = %hu\n",irqnum & 0xF);
  }
  rn=rn;
}

void PCICOM2S(char *name, int numport,int RS485)
{
   int index;
   byte start = 0x20;
   unsigned short base [4];
   char port [4] = {'A','B','C','D'};
   cprintf("%40s\n\r",name);
   result = 0;
   result += read_configuration_word(bn,df,regnum = 0x3C,&irqnum);
   irqnum &= 0xF;
   for(index = 0; index < numport; index++)
    {
       result += read_configuration_word(bn,df,regnum = start,&base[index]);
       base[index] &= 0xFFFE;
       start += 4;
       textcolor(result?RED:GREEN);
       cprintf("      COM %c:%04X  IRQ:%hu%40s\n\r", port[index],base[index], irqnum,result?"FAILED":"PASSED");
    }

  if (!result)
  {
//*
    for(index = 0; index < numport; index ++)
    {
      if(!RS485)
      {
        result= _16450IRQTest(base[index],irqnum,DebugFlag);
        textcolor(result?RED:GREEN);
        cprintf("      IRQ  COM %c %47s\n\r", port[index],result?"FAILED":"PASSED");

        result = _16450NotIRQTest(base[index],irqnum,DebugFlag);
        textcolor(result?RED:GREEN);
        cprintf("      Not IRQ  COM %c %43s\n\r", port[index],result?"FAILED":"PASSED");

        result = _16450FullDuplex(base[index],DebugFlag);
        textcolor(result?RED:GREEN);
        cprintf("      COM Test COM %c%43s\n\r",port[index],result?"FAILED":"PASSED");
      }//if !rs485
      if((RS485) && (!DebugFlag))
      {
        result = _16450ShareIRQ(base[0],base[1],base[2],base[3],0,0,0,0,irqnum,numport,index);
        textcolor(result?RED:GREEN);
        cprintf("      COM %c Output Test %40s\n\r",port[index], result?"FAILED":"PASSED");
      }
    }//end for index
//*/

    if((RS485)&&(DebugFlag))
    {
       delay(500);//allow time for see results before entering debug
       result = _16450ShareIRQDebug(base[0],base[1],base[2],base[3],0,0,0,0,irqnum,numport,index,DebugFlag);
    }
  }//end if !result
   textcolor(LIGHTGRAY);
   if (!stricmp(name,"pci-wdg-impac")) {
      printf("\nPress any key to continue with the WDoG Test...\n");
      getch();
      unsigned loop=0,done=0,pause=0,dataa=0,datab=0;
      if (DebugFlag) {
         clrscr();
         do{
            gotoxy(1,6);
            printf("Press 1 to set RTSA low, Q to set RTSA high\n"
                   "Press 2 to set RTSB low, W to set RTSB high\n\n"
                   "Press 4 to set DTRA low, R to set DTRA high\n"
                   "Press 5 to set DTRB low, T to set DTRB high\n"
                   "Press L to set RTSB,DTRB low and toggle DTRB at 400ms Intervals.\n"
                   "after setting RTSA low then high\n"
                   "pressing 2 during loop will halt it.  Pressing \n"
                   "W during halted loop will restart it.\n"
                   "pressing S will stop toggling DTRB.\n"
                   "to restart the loop, toggle DTRB from high to low, and press L\n"
                   "Press X to exit\n"
                  );
            dataa=inportb(base[0]+4);
            datab=inportb(base[1]+4);
            gotoxy(1,20);
            printf("\nRTSA:%s   DTRA:%s   RTSB:%s   DTRB:%s   LOOPING:%s   PAUSED:%s",
                   (dataa&0x02?"LOW ":"HIGH"),
                   (dataa&0x01?"LOW ":"HIGH"),
                   (datab&0x02?"LOW ":"HIGH"),
                   (datab&0x01?"LOW ":"HIGH"),
                   (loop ?"TRUE ":"FALSE"),
                   (pause?"TRUE ":"FALSE")
                  );
            printf("\nCTSA:%s   CTSB:%s",
                   (inportb(base[0]+6)&0x10?"LOW ":"HIGH"),
                   (inportb(base[1]+6)&0x10?"LOW ":"HIGH")
                  );
            printf("\n%02x",inportb(base[1]+8));
            delay(200);
            if(kbhit())
              switch(toupper(getch())){
                case 'Q':outportb(base[0]+4,dataa&~0x02);//RTSA high
                         break;
                case '1':outportb(base[0]+4,dataa|0x02);//rtsa low
                         break;
                case 'L':loop=1;pause=0;//no BREAK;, also set rtsb
                         outportb(base[0]+4,dataa|0x03);//rtsa low
                         outportb(base[1]+4,datab|0x02);//rtsb low (then high, no break)
                case 'W':outportb(base[1]+4,datab&~0x02);//rtsb high
                         if (loop) pause=0;
                         break;
                case '2':outportb(base[1]+4,datab|0x02);//rtsb low
                         if (loop) pause=1;
                         break;
                case 'R':outportb(base[0]+4,dataa&~0x01);//dtra high
                         break;
                case '4':outportb(base[0]+4,dataa|0x01);//dtra low
                         break;
                case 'T':outportb(base[1]+4,datab&~0x01);//dtrb high
                         break;
                case '5':outportb(base[1]+4,datab|0x01);//dtrb low
                         break;
                case 'S':loop=0;pause=0;
                         break;
                case 'X':done=1;
              }  //  end switch getch
            delay(200);
            delay(1);
            dataa=inportb(base[0]+4);
            datab=inportb(base[1]+4);
            gotoxy(1,20);
            printf("\nRTSA:%s   DTRA:%s   RTSB:%s   DTRB:%s   LOOPING:%s   PAUSED:%s",
                   (dataa&0x02?"LOW ":"HIGH"),
                   (dataa&0x01?"LOW ":"HIGH"),
                   (datab&0x02?"LOW ":"HIGH"),
                   (datab&0x01?"LOW ":"HIGH"),
                   (loop ?"TRUE ":"FALSE"),
                   (pause?"TRUE ":"FALSE")
                  );
            printf("\nCTSA:%s   CTSB:%s",
                   (inportb(base[0]+6)&0x10?"LOW ":"HIGH"),
                   (inportb(base[1]+6)&0x10?"LOW ":"HIGH")
                  );
            printf("\n%02x",inportb(base[1]+8));
            if (loop&!pause)
            {
              datab^=0x01;
              outportb(base[1]+4,datab);
            } //toggle DTRB
         }while(!done);  //  end do/while
      }  //  end if DebugFlag
      else {
         puts("Now performing WDoG Timeout Test\n");
         // L
         dataa=inportb(base[0]+4);
         datab=inportb(base[1]+4);

         loop=1;
         pause=0;
         //also set rtsb
         outportb(base[0] + 4, dataa | 0x03);//rtsa low
         outportb(base[1] + 4, datab | 0x02);//rtsb low (then high, no break)

         // W
         outportb(base[1] + 4, datab & ~0x02);//rtsb high
         if (loop)
            pause=0;

         delay(200);
         delay(1);
         dataa=inportb(base[0]+4);
         datab=inportb(base[1]+4);

         datab^=0x01;
         outportb(base[1]+4,datab);

         // S
         dataa=inportb(base[0]+4);
         datab=inportb(base[1]+4);

         loop=0;
         pause=0;

         delay(200);
         delay(1);
         dataa=inportb(base[0]+4);
         datab=inportb(base[1]+4);

         datab^=0x01;
         outportb(base[1]+4,datab);

         clock_t start, end;
         float timeout;
         start = clock();

         do {
            end = clock();
            timeout = (end - start) / CLK_TCK;

         } while ( ( (inportb(base[1] + 6) & 0x10) == 0x10) && (timeout < 10.0) );

         end = clock();

         textcolor((timeout < 10.0)?GREEN:RED);
         printf("Timeout occurred at %f seconds: ", timeout);
         cprintf("%s\n", (timeout < 10.0)?"PASS":"FAIL");
      }
   }  //  end if pci-wdg-impac
}

void PCICOM1S(char *name, int numport)
{
   int index;
   byte start = 0x20;
   unsigned short base [4];
   char port [4] = {'A','B','C','D'};
   cprintf("%40s\n\r",name);
   result = 0;
   result += read_configuration_word(bn,df,regnum = 0x3C,&irqnum);
   irqnum &= 0xF;
   for(index = 0; index < numport; index++)
    {
       result += read_configuration_word(bn,df,regnum = start,&base[index]);
       base[index] &= 0xFFFE;
       start += 4;
       textcolor(result?RED:GREEN);
       cprintf("      COM %c:%04X  IRQ:%hu%40s\n\r", port[index],base[index], irqnum,result?"FAILED":"PASSED");
    }


  if (!result)
  {
    index = 0;
    for(index = 0; index < numport; index ++)
    {
      result = _16450COM1SIRQTest(base[index],irqnum,DebugFlag);
      textcolor(result?RED:GREEN);
      cprintf("      IRQ  COM %c %47s\n\r", port[index],result?"FAILED":"PASSED");

      result = _16450NotIRQTest(base[index],irqnum,DebugFlag);
      textcolor(result?RED:GREEN);
      cprintf("      Not IRQ  COM %c %43s\n\r", port[index],result?"FAILED":"PASSED");

      result = _16450COM1SFullDuplex(base[index],DebugFlag);
      textcolor(result?RED:GREEN);
      cprintf("      COM Test COM %c %43s\n\r",port[index], result?"FAILED":"PASSED");

    }
  }
}

void PCIDIO(char *name, int numadd, int D48Flag, int COSFlag, int COUNTFlag, int HDFlag)
{
  result = read_configuration_word(bn,df,regnum = 0x18,&basea);
  basea &= 0xFFFE;
  result += read_configuration_word(bn,df,regnum = 0x3C,&irqnum);
  irqnum &= 0xF;
  if (!result)
  {
    int i = 0;
    printf("%12s\n\r",name);
    textcolor(LIGHTGRAY);
    cprintf("Place the tristate jumper in the TST position.\n\r");
    cprintf("Press any key to continue.\n\r");
    while(!kbhit());
    getch();
    for (i = 0; i < numadd; i++)
    {
      result=_8255Tst(basea,DebugFlag);
      textcolor(result?RED:GREEN);
      cprintf("     Port %d Tristate %36s\n\r",i,result?"FAILED":"PASSED");
      basea += 4;
      result = 0;
    }
    i = 0;
    textcolor(LIGHTGRAY);
    cprintf("Move the TST jumper to the BEN position and install WRAP PLUG.\n\r");
    cprintf("Press any key to continue.\n\r");
    while(!kbhit());
    getch();
    result = read_configuration_word(bn,df,regnum = 0x18,&basea);
    basea &= 0xFFFE;
    for(i = 0; i < numadd; i++)
    {
      result =  _8255WalkingTest(basea,NODAC,DebugFlag);
      textcolor(result?RED:GREEN);
      cprintf("     Port %d Walking Bit  %37s\n\r",i,result?"FAILED":"PASSED");

      if(COSFlag)
      {
        result = _8255COS(basea,irqnum,DebugFlag);
        textcolor(result?RED:GREEN);
        cprintf("     Port %d COS (IRQ) %40s\n\r",i,result?"FAILED":"PASSED");
      }

      result  = _8255notIENTest(basea,irqnum,DebugFlag,HDFlag);
      textcolor(result?RED:GREEN);
      cprintf("     Port %d Not IRQ %42s\n\r",i,result?"FAILED":"PASSED");

      result  = _8255IENTest(basea,irqnum,DebugFlag,D48Flag,COSFlag,HDFlag);
      textcolor(result?RED:GREEN);
      cprintf("     Port %d IEN (IRQ) %40s\n\r",i,result?"FAILED":"PASSED");

      basea+=4;
      result = 0;
    }

   if(COUNTFlag)
   {
     result = read_configuration_word(bn,df,regnum = 0x18,&basea);
     basea &= 0xFFFE;

     basec = basea + 0x10;
     if(numadd==4)
       Flag96 = 1;
     else
       Flag96 = 0;
     result = _8254CounterIEN(basea,irqnum,Flag96,DebugFlag);
 //    result = _8254CounterIEN(basea,irqnum,DebugFlag);
     textcolor(result?RED:GREEN);
     cprintf("     Counter Interrupt %39s\n\r",result?"FAILED":"PASSED");
                     //change back to 2
     for(i = 0;i <= 2; i++)
     {
       if((inportb(basec)&inportb(basec+1)&inportb(basec+2)&inportb(basec+3))!=0xFF)
       {
         result = _8254Test(basec,DebugFlag);
         textcolor(result?RED:GREEN);
         cprintf("     8254 Chip %d %45s\n\r",i,result?"FAILED":"PASSED");

       }else{
         textcolor(LIGHTGRAY);
         cprintf("No counter detected\n\r");
       }
       basec+=4;
     }//end FOR
   }//end COUNTFlag
  }//if card found
}


void PCIeDIO(char *name, int numadd, int D48Flag, int COSFlag, int COUNTFlag, int HDFlag)
{
  printf("%12s\n\r",name);
  result = read_configuration_word(bn,df,regnum = 0x14, &basea); //enable IRQ generation
  basea &= 0xFFFE;
  outportb(basea+0x69,0x09);

  result = read_configuration_word(bn,df,regnum = 0x18, &basea);
  basea &= 0xFFFE;

  result += read_configuration_word(bn,df,regnum = 0x3C, &irqnum);
  irqnum &= 0xF;

  int i = 0;
  printf("Please install the wrap plugs now.\n\r");
  textcolor(LIGHTGRAY);
  cprintf("Press any key to continue.\n\r");
  while(!kbhit());
  getch();

  if (!result)
  {
    result = read_configuration_word(bn,df,regnum = 0x18,&basea);
    basea &= 0xFFFE;
    for(i = 0; i < numadd; i++)
    {
      result =  _8255WalkingTest(basea,NODAC,DebugFlag);
      textcolor(result?RED:GREEN);
      cprintf("     Port %d Walking Bit  %37s\n\r",i,result?"FAILED":"PASSED");

      if(COSFlag)
      {
        result = _8255COS(basea,irqnum,DebugFlag);
        textcolor(result?RED:GREEN);
        cprintf("     Port %d COS (IRQ) %40s\n\r",i,result?"FAILED":"PASSED");
      }

      result  = _8255notIENTest(basea,irqnum,DebugFlag,HDFlag);
      textcolor(result?RED:GREEN);
      cprintf("     Port %d Not IRQ %42s\n\r",i,result?"FAILED":"PASSED");

      result  = _8255IENTest(basea,irqnum,DebugFlag,D48Flag,COSFlag,HDFlag);
      textcolor(result?RED:GREEN);
      cprintf("     Port %d IEN (IRQ) %40s\n\r",i,result?"FAILED":"PASSED");

      basea+=4;
      result = 0;
    }

   if(COUNTFlag)
   {
     result = read_configuration_word(bn,df,regnum = 0x18,&basea);
     basea &= 0xFFFE;

     basec = basea + 0x10;
     if(numadd==4)
       Flag96 = 1;
     else
       Flag96 = 0;
     result = _8254CounterIEN(basea,irqnum,Flag96,DebugFlag);
 //    result = _8254CounterIEN(basea,irqnum,DebugFlag);
     textcolor(result?RED:GREEN);
     cprintf("     Counter Interrupt %39s\n\r",result?"FAILED":"PASSED");
                     //change back to 2
     for(i = 0;i <= 2; i++)
     {
       if((inportb(basec)&inportb(basec+1)&inportb(basec+2)&inportb(basec+3))!=0xFF)
       {
         result = _8254Test(basec,DebugFlag);
         textcolor(result?RED:GREEN);
         cprintf("     8254 Chip %d %45s\n\r",i,result?"FAILED":"PASSED");

       }else{
         textcolor(LIGHTGRAY);
         cprintf("No counter detected\n\r");
       }
       basec+=4;
     }//end FOR
   }//end COUNTFlag
  }//if card found
  result = read_configuration_word(bn,df,regnum = 0x14,&basea);
  basea &= 0xFFFE;
  cprintf("BAR[1] @ %04X.  Writing 0x01 to BAR[1]+0x69",basea);
  outportb(basea+0x69,0x01);

}

/************************************
PCIe-DIO-24DS,
PCIe-DIO-24D,
PCIe-DIO-24DS (with counters),
PCIe-DIO-24D (with counters)
*/
void PCIeDIO24DSTest(char *name, int D48Flag, int COSFlag, int COUNTFlag, int HDFlag)
{
    textcolor(LIGHTGRAY);

    result = read_configuration_word(bn,df,regnum = 0x3C,&irqnum);
    irqnum &= 0xF;

    printf( "%12s IRQ(hex):%01X\n\r", name, irqnum );
    result += read_configuration_word( bn, df, regnum = 0x14, &baseb );
    baseb  &=  0xFFFE;
    outportb( baseb + 0x69, 0x09 );

    result += read_configuration_word(bn,df,regnum = 0x18,&basea);
    basea &= 0xFFFE;

    result += read_configuration_word(bn,df,regnum = 0x3C,&irqnum);
    irqnum &= 0xF;


    printf("Please make sure no wrap plugs are installed\n\r"
           "Press any key to continue.\n\r"
          );
    getch();

    // Test software pull-up / pull-down control
    outportb( basea + 0x03, 0x9b ); // all input mode

    outportb( basea + 0x08, 0x00 ); // all pull-downs
    result=( inportb(basea + 0x00) + inportb(basea + 0x01) + inportb(basea + 0x02) ); //all pulldowns should be zero inputs

    outportb( basea + 0x08, 0x0A ); // alternating pullups
    result += ( ( inportb(basea + 0x00) + inportb(basea + 0x01) + inportb(basea + 0x02) ) == 0x1EF) ? 0: 1; //00+FF+F0 (a+b+c)

    outportb( basea + 0x08, 0x05 ); // alternating pullups
    result += ( ( inportb(basea + 0x00) + inportb(basea + 0x01) + inportb(basea + 0x02) ) == 0x10E) ? 0: 1; //FF+00+0F (a+b+c)

    outportb( basea + 0x08, 0x0F ); // all pull-ups
    result += ( ( inportb(basea + 0x00) + inportb(basea + 0x01) + inportb(basea + 0x02) ) == 0x2FD) ? 0: 1; //all pullups should be 0xFF inputs * 3

    textcolor( result ? RED : GREEN );
    cprintf("     Software pullup/pulldown  %37s\n\r", result ? "FAILED" : "PASSED");

    result = 0;
    textcolor(LIGHTGRAY);
    printf("Please install the wrap plugs now.\n\r");
    cprintf("Press any key to continue.\n\r");
    while(kbhit()) getch();
    getch();
    outportb(basea+0x0E,0);    // disable IRQ sources other than COS
    outportb(basea+0x0B,0xFF); // mask out all COS IRQ sources
    outportb(basea+0x0F,0);    //clear any left over IRQs


    result =  _8255WalkingTest(basea,NODAC,DebugFlag);
    textcolor(result?RED:GREEN);
    cprintf("     Walking Bit  %37s\n\r",result?"FAILED":"PASSED");

    if(COSFlag)
    {
        result = _8255COS(basea,irqnum,DebugFlag);
        textcolor(result?RED:GREEN);
        cprintf("     COS (IRQ) %40s\n\r",result?"FAILED":"PASSED");
    }

    outportb( basea + 0x0E, 0x00 ); //turn off CTR and IEN IRQ sources
    result  = _8255notIENTest( basea, irqnum, DebugFlag, HDFlag );
    textcolor( result ? RED : GREEN );
    cprintf("     Not IEN IRQ %42s\n\r",result?"FAILED":"PASSED");

    outportb( basea + 0x0E, 0x01 ); //enable IEN IRQ source, disable CTR source
    result  = _8255IENTest(basea,irqnum,DebugFlag,1,1,0);
    textcolor(result?RED:GREEN);
    cprintf("     IEN (IRQ) %40s\n\r",result?"FAILED":"PASSED");

    result = 0;

    if(COUNTFlag)
    {
        basec = basea + 0x10;
        outportb( basea + 0x0E, 0x02 ); //enable CTR IRQ source, disable IEN source
        result = _8254CounterIEN(basea,irqnum,0,DebugFlag);
        textcolor(result?RED:GREEN);
        cprintf("     Counter Interrupt %39s\n\r",result?"FAILED":"PASSED");
        for(int i = 0;i <= 2; i++)
        {
            if((inportb(basec)&inportb(basec+1)&inportb(basec+2)&inportb(basec+3))!=0xFF)
            {
                result = _8254Test(basec,DebugFlag);
                textcolor(result?RED:GREEN);
                cprintf("     8254 Chip %d %45s\n\r",i,result?"FAILED":"PASSED");
            }else{
                textcolor(LIGHTGRAY);
                cprintf("No counter detected\n\r");
            }
            basec+=4;
        }//end FOR
    }//end COUNTFlag

  cprintf("BAR[1] @ %04X.  Writing 0x01 to BAR[1]+0x69",basea);
  outportb(baseb+0x69,0x01);

}



int isr_96flag=0;
int base96=0;

void interrupt setirq96flag(...)
{
  outport(base96 + 0x10,0x0000);//clear IRQ on card
  sendEOI();
  isr_96flag = 1;        /* indicate an interrupt has occurred */
}   /* end setiiroflag */

int _96bitIRQtest(int base,int irq)
{
  int ppi,port,bit,bytetowrite,addresstowrite, irqstatus;
  base96=base;
  puts("\n\rPLEASE REMOVE THE TEST JIG/WRAP PLUGs and press any key to continue.");
  getch();

  initirq(irq,setirq96flag);
/*
  0.0 Read COS status register, should be all zeros (no bits were enabled)
  1.0 Disable all COS bits
  2.0 Walking bit pattern on all 96 bits, counting IRQs (should be zero)
  3.0 Enable all COS bits
  4.0 Walking bit pattern on all 96 bits, counting IRQs (should be 16)
  5.0 Enable some COS bits
  6.0 walking bit pattern on all 96 bits, counting IRQs (should be some)
*/
  //walking on 96
  outportb(base+0,0);
  outportb(base+1,0);
  outportb(base+2,0);
  outportb(base+4,0);
  outportb(base+5,0);
  outportb(base+6,0);
  outportb(base+8,0);
  outportb(base+9,0);
  outportb(base+0xa,0);
  outportb(base+0xc,0);
  outportb(base+0xd,0);
  outportb(base+0xe,0);

  if (inport(base+0x10) != 0x0000)
    puts("Error, unexpected IRQ status - power on enabled??");
  outport(base+0x12,0x0000);//disable all COS IRQs
  puts("COS disabled");

  outportb(base+3,0x80);
  outportb(base+7,0x80);
  outportb(base+0x0B,0x80);
  outportb(base+0x0F,0x80);

  puts("Walking bit, should get no IRQs");
  for (ppi=0;ppi<4;ppi++)
    for (port=0;port<3;port++)
      for (bit=-1;bit<9;bit++)
      {
        if ((bit == -1) || (bit == 8))  //start with "0", and end with "0"
          bytetowrite = 0;
        else
          bytetowrite = (1 << bit);
        addresstowrite = base + (ppi * 4) + port;

        isr_96flag=0;
        outportb(addresstowrite,bytetowrite);

        delay(1);
        if (isr_96flag)
        {
          printf("BAD!! IRQ on PPI %d, port %d, bit %d\n\r",ppi,port,bit);
          return 1;
        }
      }
  irqstatus = inport(base+0x10);
  if ( irqstatus != 0x0000)
    printf("\n\rError, unexpected IRQ status %04x should be 0000\n\r",irqstatus);

  outport(base+0x12,0xffff);//enable all COS IRQs
  puts("COS enabled all bits (16 total)");
  puts("Walking bit, should get 1 IRQ each on all PPIs, port 2 bits 4-7");
  for (ppi=0;ppi<4;ppi++)
    for (port=0;port<3;port++)
      for (bit=-1;bit<9;bit++)
      {
        if ((bit == -1) || (bit == 8))  //start with "0", and end with "0"
          bytetowrite = 0;
        else
          bytetowrite = (1 << bit);
        addresstowrite = base + (ppi * 4) + port;

        isr_96flag=0;
        outportb(addresstowrite,bytetowrite);

        delay(1);
        if ((isr_96flag) && (bit != 8))  //don't both showing the falling edge
          printf("IRQ on PPI %d, port %d, bit %d\n\r",ppi,port,bit);
      }
  irqstatus = inport(base+0x10);

  outport(base+0x12,0x0000);
  outportb(base+3,0x9b);
  outportb(base+7,0x9b);
  outportb(base+0x0B,0x9b);
  outportb(base+0x0F,0x9b);

  restoreirq(irq);
  return 0;
}


#pragma argsused
void PCIDIOPairPair(char *name)
{
  result = read_configuration_word(bn,df,regnum = 0x18,&basea);
  basea &= 0xFFFE;
  result |= read_configuration_word(bn,df,regnum = 0x3C,&irqnum);
  irqnum &= 0xF;

  result |= _8255PairPair(basea, irqnum, DebugFlag);
//  outport(basea+0x12,0xaaaa);
//  if (inport(basea+0x12) != 0xaaaa)
//    puts("NO COS INSTALLED - SKIPPING IRQ TEST");
//  else
//  {
    result |= _96bitIRQtest(basea,irqnum);
//  }
}

#pragma argsused
void PCIAI(char *name, int channel, unsigned int FIFO)
{

  result = read_configuration_word(bn,df,regnum = 0x18,&basea);
  basea &= 0xFFFE;
  result += read_configuration_word(bn,df,regnum = 0x3C,&irqnum);
  irqnum &= 0xF;
  textcolor(LIGHTGRAY);
  cprintf("      Base:%04X  IRQ:%x\n\r",basea,irqnum);

  if(!result){
    _ai(basea,irqnum,FIFO,DebugFlag);

    if (!FIFO){
      result = aiout2(basea,irqnum,DebugFlag);
      textcolor(result?RED:GREEN);
      cprintf("     Out 2 Start %42s\n\r",result?"FAILED":"PASSED");
    }else{
      result = aisoftware(basea);
      textcolor(result?RED:GREEN);
      cprintf("  Software Start %42s\n\r",result?"FAILED":"PASSED");
    }
    result = ainotIRQ(basea,irqnum,DebugFlag);
    textcolor(result?RED:GREEN);
    cprintf("     Not IRQ %46s\n\r",result?"FAILED":"PASSED");

    if(!FIFO){
    result = aiexternal(basea,irqnum,DebugFlag);
    textcolor(result?RED:GREEN);
    cprintf("     External Start %39s\n\r",result?"FAILED":"PASSED");
    }

    result = aidigital(basea,FIFO,DebugFlag);
    textcolor(result?RED:GREEN);
    cprintf("     Digital %33s\n\r",result?"FAILED":"PASSED");
  }
}

#pragma argsused
void PCIDA(char *name, int channel)
{
  int index;
  result = read_configuration_word(bn,df,regnum = 0x18,&basea);
  basea  &=  0xFFFE;
  result = read_configuration_word(bn,df,regnum = 0x1C,&basee);
  basee  &=  0xFFFE;
  result += read_configuration_word(bn,df,regnum = 0x3C,&irqnum);
  irqnum  &=  0xF;

  textcolor(result?RED:GREEN);
  cprintf("card:%s  Base: %04X  EEPROM Base: %04X\n\r", name,basea, basee);

  if(!result)
  {

     textcolor(LIGHTGRAY);
     result= dac(basea,basee,channel,DebugFlag);

     if (channel>7){
       result = _8254Test(basea+0x24,DebugFlag);//counter base+24 hex
       textcolor(result?RED:GREEN);
       cprintf("     8254 Counter %42s\n\r",result?"FAILED":"PASSED");
     }

     result =  _8255WalkingTest(basea+=32,DAC,DebugFlag);
     textcolor(result?RED:GREEN);
     cprintf("     DAC Digital Test  %37s\n\r",result?"FAILED":"PASSED");

  }
}

void PCIWDG(char *name,int numport)
{
   int index;
   byte start = 0x18;
   unsigned short base [2];
   char port [2] = {'A','B'};
   cprintf("%40s\n\r",name);
   result = 0;
   result += read_configuration_word(bn,df,regnum = 0x3C,&irqnum);
   irqnum  &=  0xF;
   index = 0;
  // for(index = 0; index < numport; index++)
  //  {
     result += read_configuration_word(bn,df,regnum = start,&base[0]);
     base[index]  &=  0xFFFE;
     base[1]=base[0]+0x10;
     base[2]=base[1]+8;
  //   start += 8;
     textcolor(result?RED:GREEN);
     cprintf("      BASE:%04X  IRQ:%hu%40s\n\r", base[0], irqnum,result?"FAILED":"PASSED");
     cprintf("      COM A:%04X     COM B:%04X\n\r",base[1],base[2]);
    // maxport++;
   // }

   if(!result)
   {
    index = 0;
    for(index = 0; index <= numport; index ++)
    {
      if(index==0){
        //result= WDOGIRQTest(base[index],irqnum,DebugFlag);//not written
        //textcolor(result?RED:GREEN);
        //cprintf("      WDOG IRQ %47s\n\r", result?"FAILED":"PASSED");

        result = WDOGCounterTest(base[index],irqnum,DebugFlag);
        textcolor(result?RED:GREEN);
        cprintf("      WDOG Counter %43s\n\r", result?"FAILED":"PASSED");

     //   result = WDOGNotIRQTest(base[index],irqnum,DebugFlag);//not written
     //   textcolor(result?RED:GREEN);
     //   cprintf("      WDOG Not IRQ %43s\n\r", port[index],result?"FAILED":"PASSED");

        textcolor(LIGHTGRAY);
        cprintf("Install the RS-485 wrap plug and press any key to continue.\n\r");
        getch();

        result = _16450HalfDuplex(base[1],base[2],DebugFlag);
        textcolor(result?RED:GREEN);
        cprintf("      RS485 Out A In B:%39s\n\r",result?"FAILED":"PASSED");

        result = _16450HalfDuplex(base[2],base[1],DebugFlag);
        textcolor(result?RED:GREEN);
        cprintf("      RS485 Out B In A:%39s\n\r",result?"FAILED":"PASSED");

        textcolor(LIGHTGRAY);
        cprintf("Install the WDG-2S RS-422 wrap plug on each port and press any key to continue.\n\r");
        getch();


      }else{

        result = _16450IRQTest(base[index],irqnum,DebugFlag);
        textcolor(result?RED:GREEN);
        cprintf("      IRQ  COM %c %45s\n\r", port[index-1],result?"FAILED":"PASSED");

        result = _16450NotIRQTest(base[index],irqnum,DebugFlag);
        textcolor(result?RED:GREEN);
        cprintf("      Not IRQ  COM %c %41s\n\r", port[index-1],result?"FAILED":"PASSED");

        result = _16450WDGFullDuplex(base[index],DebugFlag);
        textcolor(result?RED:GREEN);
        cprintf("      COM %c  %49s\n\r",port[index-1],result?"FAILED":"PASSED");
      }
    }
   }
}

void PCICOM(char *name,int RS485,int numport)
{
   int index;
   byte start = 0x18;
   unsigned short base [4];
   char port [4] = {'A','B','C','D'};
   cprintf("%40s\n\r",name);
   result = 0;
   result += read_configuration_word(bn,df,regnum = 0x3C,&irqnum);
   irqnum  &=  0xF;
   for(index = 0; index < numport; index++)
    {
     result += read_configuration_word(bn,df,regnum = start,&base[index]);
     base[index]  &=  0xFFFE;
     start += 4;
     textcolor(result?RED:GREEN);
     cprintf("      COM %c:%04X  IRQ:%hu%40s\n\r", port[index],base[index], irqnum,result?"FAILED":"PASSED");
    // maxport++;
    }

   if(!result)
   {
    index = 0;
    for(index = 0; index < numport; index ++)
    {
       if(!RS485){

         result= _16450IRQTest(base[index],irqnum,DebugFlag);
         textcolor(result?RED:GREEN);
         cprintf("      IRQ  COM %c %47s\n\r", port[index],result?"FAILED":"PASSED");

         result = _16450NotIRQTest(base[index],irqnum,DebugFlag);
         textcolor(result?RED:GREEN);
         cprintf("      Not IRQ  COM %c %43s\n\r", port[index],result?"FAILED":"PASSED");

         result = _16450FullDuplex(base[index],DebugFlag);
         textcolor(result?RED:GREEN);
         cprintf("      COM Test COM %c%43s\n\r",port[index],result?"FAILED":"PASSED");
       }
         if((RS485)&&(!DebugFlag)){
           result = _16450ShareIRQ(base[0],base[1],base[2],base[3],0,0,0,0,irqnum,numport,index);
           textcolor(result?RED:GREEN);
           cprintf("      COM %c Output Test %40s\n\r",port[index], result?"FAILED":"PASSED");
       }
     }
      if((RS485)&&(DebugFlag)){
         delay(500);//allow time for see results before entering debug
         result = _16450ShareIRQDebug(base[0],base[1],base[2],base[3],0,0,0,0,irqnum,numport,index,DebugFlag);
      }
    }
}


void eCOM(char *name, byte numport, unsigned short *base, char *prot)
{
    char port [8] = {'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H'};
    char *p[4] ={"RS-232", "RS-422", "RS-485", "!ERROR!"};
    unsigned long PericomControlValuePerProtocolAllPorts[] = {0x00000000L, 0x11111111L, 0xFFFFFFFFL, 0};

    cprintf("%40s\n\r",name);

    for (byte protocol = 0; protocol <= 2; protocol++)
    {
        if (prot[protocol] != S_IGNORE)
        {
            cprintf("Press any key to begin %s test -- Press ESC to skip this step\n\r",p[prot[protocol]]);
            if (getch() == 27) continue;

            write_configuration_dword(bn,df,regnum = 0xB4, PericomControlValuePerProtocolAllPorts[prot[protocol]]);

            for (byte index = 0; index < numport; index++)
            {
                textcolor(result?RED:GREEN);
                cprintf("      COM %c:%04X  IRQ:%hu%40s\n\r", port[index], base[index], irqnum, "FOUND");
                switch(prot[protocol])
                {
                case S_232:
                  DTRTSTest(base, index, 0x01, 0xA0, "DTR");
                  DTRTSTest(base, index, 0x0A, 0x50, "RTS");
                case S_422:
                     result= _16450IRQTest( base[index], irqnum, DebugFlag);
                     textcolor(result?RED:GREEN);
                     cprintf("      IRQ  COM %c %47s\n\r", port[index],result?"FAILED":"PASSED");

                     result = _16450NotIRQTest(base[index],irqnum,DebugFlag);
                     textcolor(result?RED:GREEN);
                     cprintf("      Not IRQ  COM %c %43s\n\r", port[index],result?"FAILED":"PASSED");

                     result = _16450FullDuplex(base[index],DebugFlag);
                     textcolor(result?RED:GREEN);
                     cprintf("      COM Test COM %c%43s\n\r",port[index],result?"FAILED":"PASSED");
                     break;
                case S_485:
                     result = _16450ShareIRQDebug(base[0],base[1],base[2],base[3],base[4],base[5],base[6],base[7],irqnum,numport,index,DebugFlag);
                     textcolor( result ? RED : GREEN );
                     cprintf("      COM %c Output Test %40s\n\r",port[index], result?"FAILED":"PASSED");

                     break;

                }
            } // for each port
        } // if protocol is not S_IGNORE
    } // for each protocol
}


void PCIeCOM4S(char *name, int numport, byte F1, byte F2, byte F3)
{
   int index;
   int i;
   byte prot[] = {F1,F2,F3};
   byte start = 0x10;  //location of BAR[0]
   unsigned short base[8];
   char port [8] = {'A','B','C','D','E','F','G','H'};
   //maxport = 8;
   result = 0;
   result += read_configuration_word(bn,df,regnum = 0x3C,&irqnum);
   irqnum  &=  0xF;
   result += read_configuration_word(bn,df,regnum = start,&base[0]);
   base[0]  &=  0xFFFE;
   base[1]=base[0]+8;
   base[2]=base[0]+16;
   base[3]=base[0]+0x38;
   eCOM( name, numport, base, prot );
}

void PCIeCOM8S(char *name, int numport, byte F1, byte F2, byte F3)
{
   int index;
   int i;
   byte prot[] = {F1,F2,F3};
   byte start = 0x10;  //location of BAR[0]
   unsigned short base [8];
   char port [8] = {'A','B','C','D','E','F','G','H'};
   cprintf("%40s\n\r",name);
   //maxport = 8;
   result = 0;
   result += read_configuration_word(bn,df,regnum = 0x3C,&irqnum);
   irqnum  &=  0xF;
   result += read_configuration_word(bn,df,regnum = start,&base[0]);
   base[0]  &=  0xFFFE;
   base[1]=base[0]+8;
   base[2]=base[0]+16;
   base[3]=base[0]+0x18;
   base[4]=base[0]+0x20;
   base[5]=base[0]+0x28;
   base[6]=base[0]+0x30;
   base[7]=base[0]+0x38;
   eCOM( name, numport, base, prot );
}

void PCIWDGCSM(char *name)
{ result = read_configuration_word(bn,df,regnum = 0x18,&basea);
  basea  &=  0xFFFE;
  result += read_configuration_word(bn,df,regnum = 0x3C,&irqnum);
  irqnum  &=  0xF;

  if (!result){
    //test if card found
    textcolor(LIGHTGRAY);
    cprintf("%40s\n\r",name);
    textcolor(result?RED:GREEN);
    cprintf("BASE:%04X  IRQ:%hu%40s\n\r",basea, irqnum, result?"FAILED":"PASSED");
    result = _WDOGBitTest(basea,irqnum,DebugFlag);
    //result = _WDOGBuzzerTest(basea,DebugFlag);
    //result = _8254WDOGTest(basea,DebugFlag);
  }

}

void PCIIIRO(char *name)
{

  result = read_configuration_word(bn,df,regnum = 0x14,&basea); //enable IRQ generation
  basea &= 0xFFFE;
  outportb(basea+0x69,0x09);


  result = read_configuration_word(bn,df,regnum = 0x18,&basea);
  basea  &=  0xFFFE;
  result += read_configuration_word(bn,df,regnum = 0x3C,&irqnum);
  irqnum  &=  0xF;
  if (!result){
    //test if card found
    textcolor(LIGHTGRAY);
    cprintf("%40s\n\r",name);
    textcolor(result?RED:GREEN);
    cprintf("BASE:%04X  IRQ:%hu%40s\n\r",basea, irqnum, result?"FAILED":"PASSED");

    result = relay_write_test(basea,DebugFlag);
    textcolor(result?RED:GREEN);
    cprintf("      Relay Write Test: %47s\n\r",result?"FAILED":"PASSED");

    result = relay_step_test(basea,DebugFlag);
    textcolor(result?RED:GREEN);
    cprintf("      Relay Step Test: %48s\n\r",result?"FAILED":"PASSED");

    result = soft_filter_test(basea,DebugFlag);
    textcolor(result?RED:GREEN);
    cprintf("      Software Filter Test: %43s\n\r",result?"FAILED":"PASSED");

    result = manual_filter_test(basea,DebugFlag);
    textcolor(result?RED:GREEN);
    cprintf("      Manual Filter Test: %45s\n\r",result?"FAILED":"PASSED");

    result = interrupt_test(basea,irqnum,DebugFlag);
    textcolor(result?RED:GREEN);
    cprintf("      Interrupt Test: %49s\n\r",result?"FAILED":"PASSED");


  }

}

void PCIIIRO16(char *name)
{

  result = read_configuration_word(bn,df,regnum = 0x14,&basea); //enable IRQ generation
  basea &= 0xFFFE;
  outportb(basea+0x69,0x09);

  result = read_configuration_word(bn,df,regnum = 0x18,&basea);
  basea  &=  0xFFFE;
  result += read_configuration_word(bn,df,regnum = 0x3C,&irqnum);
  irqnum  &=  0xF;
  if (!result){
    //test if card found
    textcolor(LIGHTGRAY);
    cprintf("%40s\n\r",name);
    textcolor(result?RED:GREEN);
    cprintf("BASE:%04X  IRQ:%hu%40s\n\r",basea, irqnum, result?"FAILED":"PASSED");

    unsigned char RelayOffset = 8;
    do
    {
      RelayOffset -= 4;

      textcolor(LIGHTGRAY);
      cprintf("   %s Port\n\r", RelayOffset ? "HI" : "LO");
      cprintf("Install an IIRO-16 spider cable with an IIRO-16 wrap plug on the %s port.\n\r", RelayOffset ? "HI" : "LO");
      cprintf("Apply +5V to red wrap plug lead (Relay commons).\n\r");
      cprintf("Apply GND to black wrap plug lead (Isolated input lows).\n\r");
      cprintf("Press any key to start test.\n\r");
      getch();

      result = iiro16_relay_write_test(basea,DebugFlag, RelayOffset);
      textcolor(result?RED:GREEN);
      cprintf("      Relay Write Test: %47s\n\r",result?"FAILED":"PASSED");

      result = iiro16_relay_step_test(basea,DebugFlag, RelayOffset);
      textcolor(result?RED:GREEN);
      cprintf("      Relay Step Test: %48s\n\r",result?"FAILED":"PASSED");

      result = iiro16_soft_filter_test(basea,DebugFlag, RelayOffset);
      textcolor(result?RED:GREEN);
      cprintf("      Software Filter Test: %43s\n\r",result?"FAILED":"PASSED");

      result = iiro16_interrupt_test(basea,irqnum,DebugFlag, RelayOffset);
      textcolor(result?RED:GREEN);
      cprintf("      Interrupt Test: %49s\n\r",result?"FAILED":"PASSED");
    } while (RelayOffset);
  }
}

void PCIIDIO16(char *name)
{
  result = read_configuration_word(bn,df,regnum = 0x18,&basea);
  basea  &=  0xFFFE;
  result += read_configuration_word(bn,df,regnum = 0x3C,&irqnum);
  irqnum  &=  0xF;
  if (!result){
    //test if card found
    textcolor(LIGHTGRAY);
    cprintf("%40s\n\r",name);
    textcolor(result?RED:GREEN);
    cprintf("BASE:%04X  IRQ:%hu%40s\n\r",basea, irqnum, result?"FAILED":"PASSED");

    unsigned char HIPort = 4;
    unsigned char LOPort = 0;

    textcolor(LIGHTGRAY);
    cprintf("Install a PCI-IDIO wrap plug at J1 connector.\n\r");
    cprintf("Apply +5V to wrap plug - red/white lead (Output HI).\n\r");
    cprintf("Apply GND to wrap plug - black lead (Input LO) and black/white lead (Output LO).\n\r");
    cprintf("Press any key to start test.\n\r");
    getch();

    result = iiro16_relay_write_test(basea, DebugFlag, HIPort);
    textcolor(result?RED:GREEN);
    cprintf("      Relay Write Test (HI): %47s\n\r",result?"FAILED":"PASSED");
    result = iiro16_relay_write_test(basea, DebugFlag, LOPort);
    textcolor(result?RED:GREEN);
    cprintf("      Relay Write Test (LO): %47s\n\r",result?"FAILED":"PASSED");

    result = iiro16_relay_step_test(basea, DebugFlag, HIPort);
    textcolor(result?RED:GREEN);
    cprintf("      Relay Step Test (HI): %48s\n\r",result?"FAILED":"PASSED");
    result = iiro16_relay_step_test(basea, DebugFlag, LOPort);
    textcolor(result?RED:GREEN);
    cprintf("      Relay Step Test (LO): %48s\n\r",result?"FAILED":"PASSED");

    result = idio16_soft_filter_test(basea, DebugFlag, HIPort);
    textcolor(result?RED:GREEN);
    cprintf("      Software Filter Test (HI): %43s\n\r",result?"FAILED":"PASSED");
    result = idio16_soft_filter_test(basea, DebugFlag, LOPort);
    textcolor(result?RED:GREEN);
    cprintf("      Software Filter Test (LO): %43s\n\r",result?"FAILED":"PASSED");

    textcolor(LIGHTGRAY);
    cprintf("Apply +3V to wrap plug - red/white lead (Output HI).\n\r");
    cprintf("Run IRQ test at low input voltage range.\n\r");
    cprintf("Press any key to start test.\n\r");
    getch();

    result = idio16_interrupt_test(basea, irqnum, DebugFlag, HIPort);
    textcolor(result?RED:GREEN);
    cprintf("      Interrupt Test (HI): %49s\n\r",result?"FAILED":"PASSED");
    result = idio16_interrupt_test(basea, irqnum, DebugFlag, LOPort);
    textcolor(result?RED:GREEN);
    cprintf("      Interrupt Test (LO): %49s\n\r",result?"FAILED":"PASSED");
  }
}

void ShowHelp(void)
{
  char test;
  clrscr();
  textcolor(LIGHTGRAY);
  cprintf("PCI-Card Test Program\n\r");
  cprintf("--This program tests PCI cards and is designed \n\r");
  cprintf("  to run with as few keystrokes as possible.\n\r");
  cprintf("--Place wrap plugs on all ports and install the PCI card.\n\r");
  cprintf("--Run the program by typing 'pcietest' in DOS.\n\r");
  cprintf("--The program detects the card and runs the appropriate tests.\n\r");
  cprintf("--The results of each test are displayed in green or red, green\n\r");
  cprintf("  means pass and red means fail.\n\r");
  cprintf("\n\r");
  cprintf("--For COM boards, install a single 422 wrap plug before running the program.\n\r");
  cprintf("--The 232 test will fail the first time. Move the jumpers to 232,\n\r");
  cprintf("  install a single 232 wrap plug, and run the program again to test 232.\n\r");
  cprintf("--For the IRQ test, install the AUTO and ECHO jumpers for each port.\n\r");
  cprintf("\n\r");
  cprintf("--To run debug mode, type 'pcietest /d' at the command prompt.\n\r");
  cprintf("--The colors indicating pass or fail are also present in debug mode.\n\r");
  cprintf("--Debug displays the values generated during each test, which\n\r");
  cprintf("  runs until a key is pressed, moving to the next test.\n\r");

  cprintf("--Press any key to continue test.\n\r");
  getch();
}





void main(int argc, char *argv[])
{
  unsigned short IDIAddr = 0, IDOAddr = 0;
  unsigned char IDIIRQ;
  unsigned short i;

  int bIDOverflow = FALSE, bIDIMini = FALSE, bIDOMini = FALSE;
  unsigned int numcards=sizeof(list)/(sizeof(tcardlist));
  for (i=0;i<argc;i++){
    if (!strcmp(argv[i],"/?")) ShowHelp();
    if (!strcmp(argv[i],"/help")) ShowHelp();
    if (!strcmp(argv[i],"/d")) df=TRUE;
   // if (!strcmp(arg[i],"/t")) df=4;  //for software test

  }

  DebugFlag=df;//LoadParamsFromCommandLine(argc, *arg);

  clrscr();
  textcolor(LIGHTGRAY);
  printf("PCIeTest 0.30");if (df) printf(" running in DEBUG mode.");
  printf("\n");
  result=!pci_bios_present(&h,&i,&bn);
  if (result){
    cprintf("Number of PCI Busses Present: %u\n\r",bn+1);
    i=0;
    do{
      if( find_pci_device(list[i].dev,0x494f,index,&bn,&df)==SUCCESSFUL){
    if (foundcount>0) {
      puts("Please press any key to view the next card's information.");
      getch();
      clrscr();
          printf("PCIeTest 0.30");if (df) printf(" running in DEBUG mode.");
          printf("\n");
        }
        switch(list[i].type){
          case OTHER:         OTHERINFO();break;
          case COM1S:         PCICOM1S(list[i].cn,list[i].n);break;
          case COM2S:         PCICOM2S(list[i].cn,list[i].n,list[i].f1);break;
          case COM  :         PCICOM(list[i].cn,list[i].f1,list[i].n);break;
          case PericomCOM4S:  PCIeCOM4S(list[i].cn,list[i].n,list[i].f1,list[i].f2,list[i].f3);break;
          case PericomCOM8S:  PCIeCOM8S(list[i].cn,list[i].n,list[i].f1,list[i].f2,list[i].f3);break;
          case DIO  :         PCIDIO(list[i].cn,list[i].n,list[i].f1,list[i].f2,list[i].f3,list[i].f4);break;
          case eDIO :         PCIeDIO(list[i].cn,list[i].n,list[i].f1,list[i].f2,list[i].f3,list[i].f4);break;
          case DIO22:         PCIDIOPairPair(list[i].cn); break;
          case IIRO8:         PCIIIRO(list[i].cn);break;
          case IIRO16:        PCIIIRO16(list[i].cn);break;
          case IDIO16:        PCIIDIO16(list[i].cn);break;
          case WDGCSM:        PCIWDGCSM(list[i].cn);break;
          case WDG2S:         PCIWDG(list[i].cn,list[i].n);break;
          case DA:            PCIDA(list[i].cn,list[i].n);break;
          case AI:            PCIAI(list[i].cn,list[i].n,list[i].f1);break;
          case PCIeDIO24DS:   PCIeDIO24DSTest(list[i].cn,list[i].f1,list[i].f2,list[i].f3,list[i].f4);break;
          case IDI:
              if ( IDIAddr )
              {
                  bIDOverflow = TRUE;

                  textcolor(LIGHTGRAY);
                  cputs("A second IDI card was detected; the IDI/IDO tests only support one of each.\n\r");
                  cputs("Please unplug one and try again.\n\r");
              }
              else
              {
                  read_configuration_word(bn,df,regnum = 0x18,&IDIAddr);
                  IDIAddr  &=  0xFFFE;

                  read_configuration_byte(bn,df,regnum = 0x3C,&IDIIRQ);

                  textcolor(LIGHTGRAY);
                  cprintf("%s detected, and added to the IDI/IDO tests.\n\r", list[i].cn);
                  //cprintf("IDIIRQ=%02X\n\r", IDIIRQ);
                  if ( ! bIDOMini )
                  {
                      cprintf("Do you want to fully test the %s(Y/N)? ", list[i].cn);
                      while(-1)
                      {
                          char Key = getch() & 0xDF;
                          if ( Key == 'Y' )
                          {
                              cputs("Y\r\n");
                              bIDIMini = FALSE;
                              break;
                          }
                          if ( Key == 'N' )
                          {
                              cputs("N\r\n");
                              bIDIMini = TRUE;
                              break;
                          }
                      }
                  }
              }
          break;

          case IDO:
              if ( IDOAddr )
              {
                  bIDOverflow = TRUE;

                  textcolor(LIGHTGRAY);
                  cputs("A second IDO card was detected; the IDI/IDO tests only support one of each.\n\r");
                  cputs("Please unplug one and try again.\n\r");
              }
              else
              {
                  read_configuration_word(bn,df,regnum = 0x18,&IDOAddr);
                  IDOAddr  &=  0xFFFE;

                  textcolor(LIGHTGRAY);
                  cprintf("%s detected, and added to the IDI/IDO tests.\n\r", list[i].cn);
                  if ( ! bIDIMini )
                  {
                      cprintf("Do you want to fully test the %s(Y/N)? ", list[i].cn);
                      while(-1)
                      {
                          char Key = getch() & 0xDF;
                          if ( Key == 'Y' )
                          {
                              cputs("Y\r\n");
                              bIDOMini = FALSE;
                              break;
                          }
                          if ( Key == 'N' )
                          {
                              cputs("N\r\n");
                              bIDOMini = TRUE;
                              break;
                          }
                      }
                  }
              }
          break;

          case PCIA1216A:
                 PCIAI(list[i].cn,list[i].n,list[i].f1);

                 printf("%12s\n\r","PCI-A12-16A");
                 textcolor(LIGHTGRAY);
                 cprintf("Place the tristate jumper in the BTR position.\n\r");
                 cprintf("Press any key to continue.\n\r");

                 result=_8255TstPCIA1216A(basea+0x10,DebugFlag);
                 textcolor(result?RED:GREEN);
                 cprintf("     Port Tristate %36s\n\r",result?"FAILED":"PASSED");

                 textcolor(LIGHTGRAY);
                 cprintf("Place the tristate jumper in the BEN position.\n\r");
                 cprintf("Press any key to continue.\n\r");
                 getch();
                 result =  _8255WalkingTest(basea+0x10,NODAC,DebugFlag);
                 textcolor(result?RED:GREEN);
                 cprintf("     Port Walking Bit  %37s\n\r",result?"FAILED":"PASSED");

                 cprintf("The DACs are now outputting 0 counts.\n\r");
                 cprintf("Calibrate for minimum-scale output.\n\r");
                 cprintf("then press a key\n\r");
                 outport(basea+0x0c,0);
                 outport(basea+0x0e,0);
                 getch();

                 cprintf("The DACs are now outputting FFF counts.\n\r");
                 cprintf("Calibrate for maximum-scale output.\n\r");
                 cprintf("then press a key\n\r");
                 outport(basea+0x0c,0x0fff);
                 outport(basea+0x0e,0x0fff);
                 getch();

                 cprintf("The DACs are now outputting 7FF counts.\n\r");
                 cprintf("Verify MID-scale output.\n\r");
                 cprintf("then press a key\n\r");
                 outport(basea+0x0c,0x07ff);
                 outport(basea+0x0e,0x07ff);
                 getch();

               break;

          default:printf("The %lx device detected is not recognized by this program.\n"
                          "It is either newer than this program, or provided by some other\n"
                          "vendor.  Please get the latest version of PCIeTest, and retry.\n",vendev);
                  foundcount--;
                  break;
        }
        foundcount++;
        index++;
      }else {i++;index=0;}

    }while(i<numcards);

    if ( (IDIAddr || IDOAddr) && ! bIDOverflow )
    {
        textcolor(LIGHTGRAY);

        if ( IDIAddr == 0 )
            cputs("An IDO card was found, but no IDI card was found.\n\rThe IDI/IDO tests can't be performed without one of each.\n\r");
        else if ( IDOAddr == 0 )
            cputs("An IDI card was found, but no IDO card was found.\n\rThe IDI/IDO tests can't be performed without one of each.\n\r");
        else
        {
            outportb(IDIAddr + 7, 0x00); //Disable COS
            if ( bIDIMini )
                _IDO(DebugFlag, IDIAddr, IDOAddr, IDIIRQ);
            else if ( bIDOMini )
                IDI_(DebugFlag, IDIAddr, IDOAddr, IDIIRQ);
            else
                IDI_IDO(DebugFlag, IDIAddr, IDOAddr, IDIIRQ)
            ;
        }
    }

    textcolor(LIGHTGRAY);

    if (foundcount) {
      if (foundcount>1)
        cprintf("A total of %d cards have been found.\r\n",foundcount);
      else
        cprintf("\r\nOne card was found.\r\n");
    } else {
      cprintf("None of the PCI cards under consideration have been found.\r\nMake sure your card is installed.\r\n");
      if( find_pci_device(0x9050,0x10b5,0,&bn,&df)==SUCCESSFUL) cprintf("A PLX 9050 card *was* found.\r\n");
      if( find_pci_device(0x9030,0x10b5,0,&bn,&df)==SUCCESSFUL) cprintf("A PLX 9030 card *was* found.\r\n");
    }
  } else {
    cprintf("No PCI Bus compliant BIOS was found!\r\n");
  }
  cprintf("Exiting.\r\n");
}
