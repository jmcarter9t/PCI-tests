#include<stdio.h>
#include<dos.h>
#include<conio.h>
#include<graphics.h>
#include "irq0-15.h"
#include "8254.h"
#define uint unsigned int

unsigned ADDRESS = 0;
unsigned IRQ = 0;
int flag = 0;
int flag2 = 0;
int byte = 0;
int isrflag = 0;
int WDOGflag = 0;
void CtrMode(uint addr, char cntr, char mode)
{
  int ctrl = 0;
  ctrl = (cntr << 6) | 0x30 | (mode << 1);
  outportb(addr+3,ctrl);
}

void CtrLoad(uint addr ,int c,int val)
{
  outportb(addr+c,val & 0x00FF);
  outportb(addr+c,(val>>8) & 0x00FF);
}

uint CtrRead(uint addr , int c)
{
  outportb(addr+3,c<<6);
  return inportb (addr+c) + (inportb(addr+c) << 8);
}

void interrupt setcountflag(...)
{  isrflag = 1;               /* indicate an interrupt has occurred */
   outportb(ADDRESS+0xF,0x00);  //turn off irq source for dio-24
   if(flag2==1)
     outportb(ADDRESS+0x1F,0x00); //clear interrupt for dio-96CT
 //  inportb(ADDRESS);
}

#pragma argsused
unsigned int _8254CounterIEN(unsigned Base, unsigned irq, unsigned F96,int df)
{
  unsigned int CBase;
  int t;
  ADDRESS = Base;
  CBase = Base + 0x10;
  if(F96==1){
      flag2 = 1;          //for setcountflag isr
      CtrMode(CBase,0,2);
      CtrLoad(CBase,0,0x10);
  }

  do{
    CtrMode(CBase,1,2);
    CtrLoad(CBase,1,0x10);
    CtrMode(CBase,2,2);
    CtrLoad(CBase,2,0x10);
    initirq(irq,setcountflag);
    outportb(Base+0x0E,0x02);//enable COUNTER IRQ on PCIe-DIO-24DCS
    if(F96==1){
      outportb(Base+0x1E, 0x00); //enable interrupt for dio-96CT
      outportb(Base+0x1F, 0x00); //clear interrupt latch for dio-96CT
    }

    outportb(Base+0xF, 0x00);    // clear interrupt latch for dio-24
    isrflag = 0;
    outportb(0x20, 0x20);
    outportb(0xA0, 0x20);
    delay(1000);
    outportb(Base+0xD, 0x00);    // turn off interrupt for dio-24
    outportb(Base+0x0f, 0);      // clear left over IRQs
    if(F96==1)
      outportb(Base+0x1E, 0x00); //disable interrupt for dio-96CT
    outportb(0x20, 0x20);
    outportb(0xA0, 0x20);
    restoreirq(irq);
    t = isrflag;
    textcolor(t?GREEN:RED);
    cprintf("%s",t?"IRQ    ":"NoIRQ    ");
    if((80 - wherex()) < 10)
      gotoxy(1,wherey());
    t = 0;
  }while((!kbhit()) && (df)); //exit after running once if !Debug mode

  if (df) {
    getch();
    printf("\n");
  }
  if(F96==1){
    CtrMode(CBase,0,2);          //stop counters counting
    CtrMode(CBase,1,2);
    CtrMode(CBase,2,2);
    outportb(Base+0x1E,0x00); //disable interrupt for dio-96CT
  }
  if (isrflag)
    return 0;
  else
    return 1;
}


void interrupt setWDOGflag(...)
{
  WDOGflag = 1;               /* indicate an interrupt has occurred */
   inportb(ADDRESS+4);
}

#pragma argsused
int WDOGIRQTest(unsigned Base,int irq, int df)
{
  unsigned cntr1 = 0;
  unsigned cntr2 = 0;
  unsigned cntr1Read = 0;
  unsigned cntr2Read = 0;
  ADDRESS = Base;
  //outportb(Base+5,0x00);
  outportb(Base+3,0xb0);
  outportb(Base+4,0x04);    //what needs to be written here???
  CtrMode(Base,1,2);
  CtrLoad(Base,1,0x0050);
  CtrMode(Base,2,2);
  CtrLoad(Base,2,0x0010);
  inportb(Base + 4);  //clear any interrupts
  initirq(irq,setWDOGflag);
  WDOGflag = 0;
  delay(500);
  restoreirq(irq);
  CtrMode(Base,1,2);
  CtrMode(Base,2,2);
  if(WDOGflag)
    return 0;
  else
    return 1;
}

#pragma argsused
int WDOGCounterTest(unsigned Base,int irq, int df)
{
  unsigned cntr0 = 0;
  unsigned cntr1 = 0;
  unsigned cntr2 = 0;

  unsigned cntr0Read = 0;
  unsigned cntr1Read = 0;
  unsigned cntr2Read = 0;
  int x,y;
  int byte = 0;
  x=wherex()+10;
  y=wherey();
  outportb(Base+3,0xb0);
  outportb(Base+4,0xa4);    //what needs to be written here???
  CtrMode(Base,0,2);
  CtrLoad(Base,0,0x0FFF);
  CtrMode(Base,1,2);
  CtrLoad(Base,1,0x0050);
  CtrMode(Base,2,2);
  CtrLoad(Base,2,0x0010);

  cntr0 = CtrRead(Base,0);
  cntr1 = CtrRead(Base,1);
  cntr2 = CtrRead(Base,2);
  delay(200);
  cntr0Read = CtrRead(Base,0);
  delay(200);
  cntr1Read = CtrRead(Base,1);
  delay(200);
  cntr2Read = CtrRead(Base,2);
//  if(df){
    textcolor(LIGHTGRAY);
    gotoxy(x,wherey()+1);
    cprintf("Counter 0 Value     Counter 1 Value      Counter 2 Value\n\r");
    do{
      byte = 0;
      cntr0 = CtrRead(Base,0);
      cntr1 = CtrRead(Base,1);
      cntr2 = CtrRead(Base,2);
      delay(200);
      cntr0Read = CtrRead(Base,0);
      delay(200);
      cntr1Read = CtrRead(Base,1);
      delay(200);
      cntr2Read = CtrRead(Base,2);
      gotoxy(x,wherey());
      textcolor((cntr0!=cntr0Read)?GREEN:RED);
      cprintf("      %04x",cntr0Read);
      textcolor((cntr1!=cntr1Read)?GREEN:RED);
      cprintf("                %04x",cntr1Read);
      textcolor((cntr2!=cntr2Read)?GREEN:RED);//base color on initial results
      byte = inportb(Base + 4);
      cprintf("                 %04x \n\r",cntr2Read);
                              //fix text
          if ((byte & 1)==1)
             cprintf("1                   Temperature OK         \n\r");
          else
             cprintf("0                   Temperature above 120 F\n\r");
          if ((byte & 2)==2)
             cprintf("1                   Power OK                 \n\r");
          else
             cprintf("0                   Power not OK             \n\r");
          if ((byte & 4)==4)
             cprintf("1                   Power not low\n\r");
          else
             cprintf("0                   Power low    \n\r");
          if ((byte & 8)==8)
             cprintf("1                   Power not high\n\r");
          else
             cprintf("0                   Power high    \n\r");
          if ((byte & 16)==16)
             cprintf("1                   Watchdog interrupt    \n\r");
          else
             cprintf("0                   Watchdog OK           \n\r");
          if ((byte & 32)==32)
             cprintf("1                   Relay OK       \n\r");
          else
             cprintf("0                   Relay triggered\n\r");
          if ((byte & 64)==64)
             cprintf("1                   No interrupt    \n\r");
          else
             cprintf("0                   Interrupt       \n\r");
          if ((byte & 128)==128)
             cprintf("1                   Ground not OK \n\r");
          else
             cprintf("0                   Ground OK     \n\r");



      gotoxy(x,wherey()-9);
    }while(!kbhit());
    printf("\n");
    gotoxy(1,wherey()+9);
    getch();
 // }

  CtrMode(Base,0,2);
  CtrMode(Base,1,2);
  CtrMode(Base,2,2);
  inportb(Base + 4); //clear any interrupts
  if((cntr0!=cntr0Read)&&(cntr1!=cntr1Read)&&(cntr2!=cntr2Read))
     return 0;
  else
     return 1;
}


int _8254Test(unsigned Base,int df)
{
  unsigned cntr1 = 0;
  unsigned cntr2 = 0;
  unsigned cntr1Read = 0;
  unsigned cntr2Read = 0;
  int x,y;
  x=wherex()+10;
  y=wherey()+2;
  outportb(Base+0xE,0x00); //disable interrupt for dio-96CT
  CtrMode(Base,0,2);
  CtrLoad(Base,0,0x10);
  CtrMode(Base,1,2);
  CtrLoad(Base,1,0x100);
  CtrMode(Base,2,2);
  CtrLoad(Base,2,0xFFFF);
  delay(1);
  cntr1 = CtrRead(Base,1);
  cntr2 = CtrRead(Base,2);
  delay(200);
  cntr1Read = CtrRead(Base,1);
  cntr2Read = CtrRead(Base,2);
  if(df){
    textcolor(LIGHTGRAY);
    gotoxy(x,wherey());
    cprintf("Counter 1 Value      Counter 2 Value\n");
    do{
      cntr1Read = CtrRead(Base,1);
      delay(200);
      cntr2Read = CtrRead(Base,2);
      gotoxy(10,wherey());
      textcolor((cntr1!=cntr1Read)?GREEN:RED);//base color on initial results
      cprintf("     %4x                 %4x   ",cntr1Read,cntr2Read);
     // gotoxy(x,y-1);
    }while(!kbhit());
    printf("\n");
    getch();
  }
  outportb(Base+0xE,0x00); //disable interrupt for dio-96CT

  if((cntr1!=cntr1Read)&&(cntr2!=cntr2Read))
     return 0;
  else
     return 1;
}


void SetCounter(void)//from CSamples, sample 1 for pciwdgcsm
{
  inportb(ADDRESS+7);  //disable counter functions
  CtrMode(ADDRESS,2,0); //set counter 2 to mode 0 -- clear reset
  outportb(ADDRESS+7,0); //enable counters
  inportb(ADDRESS+7);  //disable counter functions
 // inportb(ADDRESS+6); //disable buzzer
  outportb(ADDRESS+6,0);  //enable buzzer
  outportb(ADDRESS+0x0c,0); //select low clock rate
  inportb(ADDRESS+0x0d);  //disable opto reset
  inportb(ADDRESS+0x0e);  //disable opto reset
  inportb(ADDRESS+0x0f);  //disable program outputs

  CtrMode(ADDRESS, 0, 3); // set counter 0 to mode 3
  CtrLoad(ADDRESS, 0, 0xFFFF); // load counter 0 with full scale

  CtrMode(ADDRESS, 1, 2); // set counter 1 to mode 2
  CtrLoad(ADDRESS, 1, 0x0050); // load counter 1 with 50 hex

  CtrMode(ADDRESS, 2, 1); // set counter 2 to mode 1
}//end SetCounter

void interrupt setintflag()
{
  flag=1;  //read character
  byte = inportb(ADDRESS+4);
  outportb(0xA0,0x20);  //disable interrupt for upper irq
  outportb(0x20,0x20);  //disable interrupt for lower irq
 }//end setflag

void IntroScreen(void)
{
  clrscr();
  puts("PCIWDG       Test Required");
  puts("Basic:       Verify Relay contacts go from NO to NC, NCTRGATE goes HI,");
  puts("             Q4-b turns on and Fout (U21-8) goes LO at Time Out.");
  puts("Option 1:    Verify status bits 5 and 6 go LO by shorting R31 and R37");
  puts("Option 2:    Verify status bit 1 goes LO by shorting R26.");
  puts("             Adjust RP1 for room temperature. (75 deg F = 0.75VDC)");
  puts("Option 3:    Verify the value for temperature on the display.");
  puts("Option 4:    Verify status bits 2 and 3 change when opto-inputs change.");
  puts("             Verify buzzer sounds, J2 goes HI, and opto-outputs change");
  puts("             at Time-Out.");
  puts("Option 6:    Verify status bit 4 is LO.");
  puts("             Connect fan to TB2-1&2.");
  puts("             Start the fan by momentarily shorting TB2-3 to TP +2.5V.");
  puts("             Verify status bit 4 goes HI.");
  puts("             Verify pulse width modulation increases as RP1 is turned to CW.");
  puts("             Return RP1 to correct setting.");
  printf("\n\nPress any key to start test.\n");
}

#pragma argsused
int _WDOGBitTest(unsigned base,unsigned irqnum,int df)
{
  //non debug code is taken from pwdg-csm sample1
  int i;
  float Tmp;
  unsigned long int timeout = 4000000;
  int aborted;
  ADDRESS = base;//set global ADDRESS for ISR
  textcolor(LIGHTGRAY);
  IntroScreen();
  getch();
  puts("Press any key to stop test");
  inportb(base+4);   //clear any potentially pending interrupts
  //initirq(irqnum,setintflag);  //initialize the irq
  SetCounter();  //initialize counter
  inportb(base+4);   //clear any potentially pending interrupts
  outportb(base+7,0);  //enable counters
  if(df){ //loops until keypressed in debug mode
      delay(50);
      textcolor(LIGHTGRAY);
      cprintf("Bit Value           Status Register\n\r");
      for(i = 0;!kbhit(); i++){
         inportb(base+4);//clear pending interrupts so it won't lockup
         delay(100);
         CtrMode(base,1,2); //set counter 1 to mode 2
         CtrLoad(base,1,0x0050);  //load counter 1 with 20 hex
         printf("Updating counter 1, Loop number %3d\n",i);
         byte = inportb(base+4);
         printf("\nValue in status register: %x\n",byte);
          if ((byte & 1)==1)
             cprintf("1                   Watchdog OK       \n\r");
          else
             cprintf("0                   Watchdog timed out\n\r");
          if ((byte & 2)==2)
             cprintf("1                   Temperature OK         \n\r");
          else
             cprintf("0                   Temperature above 120 F\n\r");
          if ((byte & 4)==4)
             cprintf("1                   5VDC on input #1\n\r");
          else
             cprintf("0                   0VDC on input #1\n\r");
          if ((byte & 8)==8)
             cprintf("1                   5VDC on input #0\n\r");
          else
             cprintf("0                   0VDC on input #0\n\r");
          if ((byte & 16)==16)
             cprintf("1                   Fan good                     \n\r");
          else
             cprintf("0                   Fan speed out of range     \n\r");
          if ((byte & 32)==32)
             cprintf("1                   Voltage supplies OK       \n\r");
          else
             cprintf("0                   One of three supplies high\n\r");
          if ((byte & 64)==64)
             cprintf("1                   Voltage supplies OK       \n\r");
          else
             cprintf("0                   One of three supplies low\n\r");
          if ((byte & 128)==128)
             cprintf("1                   Interrupt   \n\r");
          else
             cprintf("0                   No interrupt\n\r");
         Tmp = (inportb(base+5) * (11.0 / 15.0)) + 7;//calculate temp
         printf("Temperature inside computer: %5.1f\n", Tmp);
         gotoxy(wherex(),wherey()-12);
      }
    getch();//get keypressed
  }
    if(!df){
      for(i = 0;i<=100; i++){
         delay(100);
         textcolor(LIGHTGRAY);
         CtrMode(base,1,2); //set counter 1 to mode 2
         CtrLoad(base,1,0x0020);  //load counter 1 with 50 hex
         printf("Updating counter 1, Loop number %3d\n",i);
         byte = inportb(base+4);
         printf("\nValue in status register: %x\n",byte);
          if ((byte & 1)==1)
             cprintf("1                   Watchdog OK       \n\r");
          else
             cprintf("0                   Watchdog timed out\n\r");
          if ((byte & 2)==2)
             cprintf("1                   Temperature OK         \n\r");
          else
             cprintf("0                   Temperature above 120 F\n\r");
          if ((byte & 4)==4)
             cprintf("1                   5VDC on input #1\n\r");
          else
             cprintf("0                   0VDC on input #1\n\r");
          if ((byte & 8)==8)
             cprintf("1                   5VDC on input #0\n\r");
          else
             cprintf("0                   0VDC on input #0\n\r");
          if ((byte & 16)==16)
             cprintf("1                   Fan good                     \n\r");
          else
             cprintf("0                   Fan speed out of range     \n\r");
          if ((byte & 32)==32)
             cprintf("1                   Voltage supplies OK       \n\r");
          else
             cprintf("0                   One of three supplies high\n\r");
          if ((byte & 64)==64)
             cprintf("1                   Voltage supplies OK       \n\r");
          else
             cprintf("0                   One of three supplies low\n\r");
          if ((byte & 128)==128)
             cprintf("1                   Interrupt   \n\r");
          else
             cprintf("0                   No interrupt\n\r");
         Tmp = (inportb(base+5) * (11.0 / 15.0)) + 7;//calculate temp
         printf("Temperature inside computer: %5.1f\n", Tmp);
         gotoxy(wherex(),wherey()-12);
      }
    }
  gotoxy(wherex(),wherey()+11);
  printf("\nWaiting for timeout\n");
  while((byte=(inportb(base+4) & 1)) == 1);
  if((byte & 1)==1)
    puts("Watchdog did not time out");
  else
    puts("Watchdog timed out");
  byte=inportb(base+4);//read register
  printf("\nValue in status register: %x\n",byte);//print inportb(base+4) from ISR
  delay(200);
  if (df) {puts("Press any key to turn off reset and exit");getch();}
  puts("Exiting test!                  ");
  inportb(base+7);  //disable counters
  delay(100);

  CtrMode(ADDRESS, 2, 0); // set counter 2 to mode 0
  outportb(base + 5,0); //turn off buzzer
  delay(100);
  outportb(base + 6,0);
  //restoreirq(irqnum);
  return 0;
}

#pragma argsused
int _WDOGBuzzerTest(unsigned base, int df)
{
  puts("Press any key to start buzzer");
  getch();
 // outportb(base + 6,0); //enable buzzer
  outportb(base + 4,0); //start buzzer
  puts("Press any key to stop buzzer");
  getch();
  outportb(base + 5,0); //stop buzzer
  inportb(base + 6); //disable buzzer
  return 0;
}

