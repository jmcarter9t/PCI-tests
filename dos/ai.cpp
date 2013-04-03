//This test polls all channels for data and prints the data to the screen
//for the non-FIFO version of the card
//For the FIFO version, the card interrupts on FIFO half full and the data
//is read out of the FIFO and displayed on screen.
#include <conio.h>
#include <stdlib.h>
#include <math.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <dos.h>
#include <io.h>
#include <dos.h>
#include "irq0-15.h"
#include "8254.h"
#include "ai.h"
#define TRUE 1
#define FALSE 0
int speed = 0;
char *rangestr;
int aiflag = 1;
int newflag = 0;
unsigned IRQBASE;
unsigned int irqdata[2000];

void RESETFIFOS(unsigned int BASE)
{
  int i;
  outportb(BASE+4, 0x48);//0x40 resets channel fifo, 0x08 resets data fifo
  inportb(BASE+6);
  for (i=0;i<0x7000;i++)//totally redundant method of clearing data fifo.
    inport(BASE+0);
}

void SETCHANNEL(unsigned int BASE,unsigned int RANGE)
{
  outport(BASE+2,(0x0000)|RANGE);
  outport(BASE+2,(0x1010)|RANGE);
  outport(BASE+2,(0x2020)|RANGE);
  outport(BASE+2,(0x3030)|RANGE);
  outport(BASE+2,(0x4040)|RANGE);
  outport(BASE+2,(0x5050)|RANGE);
  outport(BASE+2,(0x6060)|RANGE);
  outport(BASE+2,(0x7070)|RANGE);
  outport(BASE+2,(0x8080)|RANGE);
  outport(BASE+2,(0x9090)|RANGE);
  outport(BASE+2,(0xA0A0)|RANGE);
  outport(BASE+2,(0xB0B0)|RANGE);
  outport(BASE+2,(0xC0C0)|RANGE);
  outport(BASE+2,(0xD0D0)|RANGE);
  outport(BASE+2,(0xE0E0)|RANGE);
  outport(BASE+2,(0xF0F0)|RANGE);
  inport(BASE + 2);
}
void STARTCONVERSION(unsigned int BASE)
{
  outportb(BASE+0, 0);//write anything to base+0 to start conversion
}

int RETRIEVEANALOGCONVERSION(unsigned int BASE)
{
  int data=0;
  data = inport(BASE+0)&0x0fff;//mask upper nibble (channel)
  return data;
}

unsigned int WAITFOREOC(unsigned int BASE)
{
  unsigned int timeout=65535;
  while(((inportb(BASE+4) & 0x80) != 0x80) && timeout--);
  return timeout; //0==error
}

void ENABLECOUNTERS(unsigned int BASE)
{
  outportb(BASE+4, 0x05); //set counter/irq enable bits, starts conversions
}

void DISABLECOUNTERS(unsigned int BASE)
{
  outportb(BASE+4, 0); //set counter disable bit, stop conversions
}

void interrupt setaiflag(...) //FIFO half-full interrupt
{
  int i;
  if (aiflag){//stop acquisition if the display routines would be interrupted.
    aiflag=99;
    DISABLECOUNTERS(IRQBASE);
  }else{
    for(i = 0; i < 1024; i++){
      irqdata[i] = inport(IRQBASE + 0);
    }
  }
  aiflag = 1;
  inportb(IRQBASE+4);
  sendEOI();
}

void interrupt setai(...) //EOC interrupt
{
  int i;
  aiflag = 0;
  inportb(IRQBASE + 4); //clear interrupt
  sendEOI();
}
unsigned int aidigital(unsigned int Base, unsigned int A, int df)
{
  int index,dis;
  unsigned char value=0;
  unsigned char value2=0;
  unsigned int result = 0;
  outportb(Base+5,0xE);
  if(df)                    //base+5 digital
  {
    printf("\n");
    gotoxy(1,wherey());
    textcolor(LIGHTGRAY);
    gotoxy(10,wherey());
    cprintf("  Value out       Value in \n");
    do{
     for(index = 0; index < 4; index++)
     {
      outportb(Base+5,1<<index);  //write value
	   delay(200);
      value = inportb(Base + 5);  //read back value
      value = (~(value)&0x0f);    //complement value because bits are inverted
      delay(100);
      for (dis = 0;dis < 4;dis++)
      {
        gotoxy(20-dis,wherey());
        textcolor(GREEN);
        putch(((value)&(1<<dis))?'1':'0');
        gotoxy(35-dis,wherey());
        textcolor(((value)==(1<<index))?GREEN:RED); //mask upper nybble
	    putch((value&(1<<dis))?'1':'0');

      }
      if (value != 1<<index)
         result++;
     }
    }while(!kbhit()&&df);
    getch();
    printf("\n");
  }
  if(!df)
  {
    for(index = 0; index < 4; index++)
    {
      value2 = (1<<index);    //debug
      outportb(Base+5,value2);  //write value
      delay(30);
      value = inportb(Base + 5);  //read back value
      value = (~(value)&0x0f);    //complement value because bits are inverted
      if (value != 1<<index)
         result++;
    }
  }
 //base+3 digital
 if(!A){                 //only run base+3 digital test on non-fifo board
   if(df)
  {
    printf("\n");
    gotoxy(1,wherey());
    textcolor(LIGHTGRAY);
    gotoxy(10,wherey());
    cprintf("  Value out       Value in \n");
    do{
     for(index = 0; index < 8; index++)
     {
      if(index!=3){                 //bit 3 not used
        outportb(Base+3,1<<index);  //write value
	     delay(200);
        value = inportb(Base + 3);  //read back value
        delay(100);
        for (dis = 0;dis < 8;dis++)
        {
          if(dis==3){
            gotoxy(20-dis,wherey());
            putch(' ');           //bit 3 not used
            gotoxy(35-dis,wherey());
            putch(' ');
          }
          else{
            gotoxy(20-dis,wherey());
            textcolor(GREEN);
            putch(((inportb(Base+3))&(1<<dis))?'1':'0');
            gotoxy(35-dis,wherey());
            textcolor((value==(1<<index))?GREEN:RED);
	         putch((value&(1<<dis))?'1':'0');
          }

        }
        if (value != 1<<index)
           result++;
      }
     }
    }while(!kbhit()&&df);
    getch();
    printf("\n");
  }
  if(!df)
  {
    for(index = 0; index < 8; index++)
    {
      if(index!=3){               //bit 3 not used
        outportb(Base+3,1<<index);  //write value
        delay(30);
        value = inportb(Base + 3);  //read back value
        if (value != 1<<index)
           result++;
      }
    }
  }
 }//end non fifo loop
  return result;
}

void Fifo(unsigned int BASE,unsigned int irq, unsigned int RANGE)
{
  unsigned int chan;
  double display = 0.0;
  int data = 0;
  int x;
  unsigned long a=0;
  int y=0;
  char msg[256];
  double span[10]={20.0,10.0,5.0,2.5,10,5,2.5,5.0};
  char *volts[]={"9.9","4.9","2.4","1.2","9.9","4.8","3.7","4-20mA"};//fix last entry
  char *str[]={"ñ10V","ñ5V","ñ2.5V","ñ1.25V","0-10V","0-5V","1.25-3.75V","4-20mA"};//fix last entry
  IRQBASE = BASE;

  clrscr();
  printf("FIFO test     Apply %7s volts     Range:%10s",volts[RANGE&0x07],str[RANGE&0x07]);
  gotoxy(1,4);
  puts("Press any key to continue.");
  RESETFIFOS(BASE);//clear channel and data fifos
  SETCHANNEL(BASE,RANGE); // set up point list
  sendEOI();
  initirq(irq,setaiflag);
  sendEOI();
  CtrMode(BASE+8,1,2);
  CtrLoad(BASE+8,1,0xff);
  CtrMode(BASE+8,2,2);
  CtrLoad(BASE+8,2,0x2);
  ENABLECOUNTERS(BASE);//set INT on fifo half full bit, enable counters to start conversions
  aiflag=0;
  while(!kbhit() && (aiflag!=99))
  {
    if(aiflag){
      for(x = 0; x < 1024; x++){
	     gotoxy(1,y+8);
	     y++;
	     y%=16;
        data=(RANGE < 4)?(((int)(int)irqdata[x] << 4) >> 4):irqdata[x]&0x0fff;
		  display = ((double)data*(span[RANGE&0x07]/4096.0));
		  if (RANGE > 5)//1.25 to 3.75 and 1.25 to 6.25
		    display+=1.25L;
	     printf("Channel:%2u   Volts: % 3.3f   Sample number: %6lu",irqdata[x]>>12,display/*data*(span[RANGE&0x07]/4096.0)*/,a);
	     a++;
      }
      aiflag = 0;
    }// end if
    ENABLECOUNTERS(BASE);
  }//end while
  if (aiflag==99) puts("DATA RATE TOO FAST FOR DISPLAY!");
  DISABLECOUNTERS(BASE);
  restoreirq(irq);
  getch();
}

void NoFifo(unsigned int BASE,unsigned int RANGE)
{
  unsigned int chan;
  int data=0;
  unsigned int r = 0;
  float display=0;
  unsigned long timeout;
  double span[10]={20.0,10.0,5.0,2.5,10,5,2.5,5.0};
  char *volts[]={"9.9","4.9","2.4","1.2","9.9","4.8","3.7","6.2"};//fix last entry
  char *str[]={"ñ10V","ñ5V","ñ2.5V","ñ1.25V","0-10V","0-5V","1.25-3.75V","4-20mA"};//fix last entry
  clrscr();
  printf("Non-FIFO test     Apply %7s volts     Range:%10s",volts[RANGE&0x07],str[RANGE&0x07]);
  gotoxy(1,4);
  puts("Press any key to continue.");
  DISABLECOUNTERS(BASE);
  while (!kbhit()) {
    delay(250);
    gotoxy(1,6);
    for (chan = 0; chan < ((RANGE==15)?8:16); chan++) {
	   timeout = 655350L;
      r=((chan<<4)|RANGE);
	   outport(BASE+2,r);//set channel/range
	   outport(BASE+3,0x0000); //remove later
	   delay(1);
	   outportb(BASE+0,0);//start conversion
	   timeout = WAITFOREOC(BASE); //WAITFOREOC returns zero if it times out
	   if (timeout == 0)
	     printf("A/D timeout  ");
	   else
	     clreol();
	   data = RETRIEVEANALOGCONVERSION(BASE); //read counts
		delay(1);
		data = (RANGE<4)?(data<<4)>>4:data;  //if bipolar sign extend
	   display = ((double)data * (span[RANGE&0x07]/4096.0));//convert to volts
		if (RANGE==6)//1.25 to 3.75
		  display+=1.25L;
		else
		  if (RANGE==15)
			 display+=1.25L;//1.25 to 6.25
	   printf("Channel: %2hu\t Counts: %4i\t Volts: % 2.3f\n", chan,data,display);
    }//end for loop
  }//end while loop
  getch();
}

void DebugFifo(unsigned int BASE, unsigned int irq)
{
  unsigned int channel;
  int data = 0;
  int x;
  double span = 0.0;
  unsigned int r = 0;
  unsigned long a=0;
  double display = 0.0;
  int y=0;
  char msg[256];
  char *rangestr;
  char key;
  int done=0;
  IRQBASE = BASE;

  clrscr();
  rangestr = "+/-10V";
  span = 20.0;
  puts("PCI-AI12-16A Debug");
  RESETFIFOS(BASE);//clear channel and data fifos
  SETCHANNEL(BASE,r); // set up point list, range set to +/-10V to start
  sendEOI();
  initirq(irq,setaiflag);
  sendEOI();
  aiflag = 0;
  CtrMode(BASE+8,1,2);
  CtrLoad(BASE+8,1,0xff);
  CtrMode(BASE+8,2,2);
  CtrLoad(BASE+8,2,0x2);
  ENABLECOUNTERS(BASE);//set INT on fifo half full bit, enable counters to start conversions
  puts("Enter the input range or press enter to keep the old range.");
  puts("Output is on all channels.");
  puts("A      +/-10V");
  puts("B      +/-5V");
  puts("C      +/-2.5V");
  puts("D      +/-1.25V");
  puts("E      0-10V");
  puts("F      0-5V");
  puts("G      1.25-3.75V");
  puts("H      1.25-6.25V (current mode, 4-20mA)");
  puts("Q      Exit program");
  while((!done) && (aiflag!=99))
  {
    if(kbhit()){
      key = getche();
    switch(key){
      case 'A': case 'a': r = 0; span = 20.0; SETCHANNEL(BASE,r); rangestr="+/-10V";break;
      case 'B': case 'b': r = 1; span = 10.0; SETCHANNEL(BASE,r); rangestr="+/-5V";break;
      case 'C': case 'c': r = 2; span = 5.0; SETCHANNEL(BASE,r); rangestr="+/-2.5V";break;
      case 'D': case 'd': r = 3; span = 2.5; SETCHANNEL(BASE,r); rangestr="+/-1.25V";break;
      case 'E': case 'e': r = 4; span = 10.0; SETCHANNEL(BASE,r); rangestr="0-10V";break;
      case 'F': case 'f': r = 5; span = 5.0; SETCHANNEL(BASE,r); rangestr="0-5V";break;
      case 'G': case 'g': r = 6; span = 2.5; SETCHANNEL(BASE,r); rangestr="1.25-3.75V";break;
	   case 'H': case 'h': r = 0xf; span = 5.0; SETCHANNEL(BASE,r); rangestr="1.25-6.25V, 4-20mA";break;
      case 'Q': case 'q': done = 1; break;
      default: r = 0; break;
    }
    if(aiflag){
//      aiflag = 0;
      printf("Range: %s\n",rangestr);
      for(x = 0; x < 1024; x++){
        gotoxy(1,y+7);
        y++;
        y%=16;
		  data=(r<4)?(((int)(int)irqdata[x] << 4) >> 4):irqdata[x]&0x0fff;//sign extend if bipolar
		  display = ((double)data*(span/4096.0));
		  if (r > 5)//1.25 to 3.75 and 1.25 to 6.25
			 display+=1.25L;
	     printf("Channel:%2u   Volts: % 3.3f   Sample number: %6lu",irqdata[x]>>12,display/*data*(span[RANGE&0x07]/4096.0)*/,a);
//		  printf("Channel:%2u   Counts: %04x  Volts: % 3.3lf   Sample number: %6lu",irqdata[x]>>12,data&0x0fff,display,a);
		  a++;
      }
      ENABLECOUNTERS(BASE);
      aiflag = 0;//this allows resumption of data taking after display, so the
               //buffer being used by the display routine is not overwritten
               //while it is being used.  BETTER would be to swap between two
               //buffers.
	 }// end if
  }//end while
}  if (aiflag==99) puts("DATA RATE TOO FAST FOR DISPLAY!");
  DISABLECOUNTERS(BASE);
  restoreirq(irq);
/*  while(!kbhit() && (aiflag!=99))
  {
    if(aiflag){
      for(x = 0; x < 1024; x++){
	     gotoxy(1,y+8);
	     y++;
	     y%=16;
        data=(RANGE < 4)?(((int)(int)irqdata[x] << 4) >> 4):irqdata[x]&0x0fff;
		  display = ((double)data*(span[RANGE&0x07]/4096.0));
		  if (RANGE > 5)//1.25 to 3.75 and 1.25 to 6.25
		    display+=1.25L;
	     printf("Channel:%2u   Volts: % 3.3f   Sample number: %6lu",irqdata[x]>>12,display/*data*(span[RANGE&0x07]/4096.0)*///,a);
/*	     a++;
      }
      aiflag = 0;
    }// end if
    ENABLECOUNTERS(BASE);
  }//end while
  if (aiflag==99) puts("DATA RATE TOO FAST FOR DISPLAY!");
  DISABLECOUNTERS(BASE);
  restoreirq(irq);
  getch();
*/
puts("");
}


void DebugNoFifo(unsigned int BASE)
{
  unsigned int channel;
  double span = 0.0;
  int data=0;
  int done = 0;
  unsigned int r = 0;
  float display=0;
  unsigned int timeout;
  unsigned int chan;
  char key;
  char *rangestr;
  clrscr();
  rangestr = "+/-10V";
  span = 20.0;
  puts("PCI-AI12-16 Debug ");
  puts("This test will not work correctly if the FIFOs are installed.");
  gotoxy(1,4);
  DISABLECOUNTERS(BASE);
  puts("Enter the input range or press enter to keep the old range.");
  puts("A      +/-10V");
  puts("B      +/-5V");
  puts("C      +/-2.5V");
  puts("D      +/-1.25V");
  puts("E      0-10V");
  puts("F      0-5V");
  puts("G      1.25-3.75V");
  puts("H      1.25-6.25V (current mode, 4-20mA)");
  puts("Q      Exit program");
  gotoxy(1,16);
  while (!done) {
     if(kbhit())
       key=getch();
     switch(key){
       case 'A': case 'a': r = 0; span = 20.0;rangestr="+/-10V";break;
       case 'B': case 'b': r = 1; span = 10.0;rangestr="+/-5V";break;
       case 'C': case 'c': r = 2; span = 5.0;rangestr="+/-2.5V";break;
       case 'D': case 'd': r = 3; span = 2.5;rangestr="+/-1.25V";break;
       case 'E': case 'e': r = 4; span = 10.0;rangestr="0-10V";break;
       case 'F': case 'f': r = 5; span = 5.0;rangestr="0-5V";break;
       case 'G': case 'g': r = 6; span = 2.5;rangestr="1.25-3.75V";break;
	    case 'H': case 'h': r = 15; span = 5.0;rangestr="1.25-6.25V, 4-20mA";break;
       case 'Q': case 'q': done = 1; break;
       default: r = 0; break;
     }
     delay(250);
//     for (chan = 0; chan < 16; chan++) {
        timeout = 65535;
        outport(BASE+2,(0|r));//set channel0 and user-selected range
        delay(1);
        outportb(BASE+0,0);//start conversion
        timeout = WAITFOREOC(BASE); //WAITFOREOC returns zero if it times out
        if (timeout == 0)
          printf("A/D timeout  ");
		  else
          clreol();

        data = RETRIEVEANALOGCONVERSION(BASE+0);
		  delay(1);
		  data = (r>3)?data:(data<<4)>>4;  //if bipolar sign extend

		  display = ((double)data * (span/4096.0));//convert to volts
		  if (r==6)//1.25 to 3.75
			 display+=1.25L;
		  else
			 if (r==15)
				display+=1.25L;//1.25 to 6.25

		  printf(" Range: %10s\t Counts: %4x\t Volts: % 2.3f\n", rangestr,data,display);
//     }//end for loop
     gotoxy(1,16);
  }//end while loop
}

void _ai(unsigned int BASE, unsigned int irqnum, unsigned int A, int df)
{
  IRQBASE = BASE;
  clrscr();
  if(df){
    if(A) //              FIX LATER
      DebugFifo(BASE,irqnum);
    else
      DebugNoFifo(BASE);
  }else{// not df
      if(A){
        Fifo(BASE,irqnum,0);  // +/-10V
        Fifo(BASE,irqnum,4);  // 0-10V
        Fifo(BASE,irqnum,1);  // +/-5V
        Fifo(BASE,irqnum,5);  // 0-5V
        Fifo(BASE,irqnum,2);  // +/-2.5V
        Fifo(BASE,irqnum,6);  // 1.25-3.75V
        Fifo(BASE,irqnum,3);  // +/-1.25V
        Fifo(BASE,irqnum,15); // 1.25-6.25 (4-20mA), set in differential mode
      }else{
        NoFifo(BASE,0);  // +/-10V
        NoFifo(BASE,4);  // 0-10V
        NoFifo(BASE,1);  // +/-5V
        NoFifo(BASE,5);  // 0-5V
        NoFifo(BASE,2);  // +/-2.5V
        NoFifo(BASE,6);  // 1.25-3.75V
        NoFifo(BASE,3);  // +/-1.25V
        NoFifo(BASE,15); // 1.25-6.25 (4-20mA), set in differential mode
      }//end if A
  }//end else df
} //end _ai

unsigned aisoftware(unsigned BASE) //add debug stuff
{

//  if(df){
//    gotoxy(1,wherey());
//    puts("Need text????");
//  }
  outport(BASE+2,0);//set channel/range
  delay(1);
  outportb(BASE+0,0);//start conversion
  return ! WAITFOREOC(BASE); //WAITFOREOC returns zero if it times out
}


unsigned int aiout2(unsigned int Base,unsigned int IRQ,int df)
{
  IRQBASE = Base;
  if(df){
    gotoxy(1,wherey()+1);
    puts("Out2 Start Conversion Test");
    puts("Connect a scope to pin 13. Press any key to exit.");
    while(!kbhit()){
      sendEOI();
      initirq(IRQ,setai);
      aiflag = 1;               //set aiflag to 1 because initirq generates interrupt
      sendEOI();
      inportb(Base+4);          //clear interrupts
      outport(Base+2,0);        //channel 0, +/-10V
      outport(Base+3,0);        //remove later
      outportb(Base+4,5);        //start conversion on out 2, enable interrupts
      CtrMode(Base+8,1,2);
      CtrLoad(Base+8,1,0x2);
      CtrMode(Base+8,2,2);
      CtrLoad(Base+8,2,0xFF);
      delay(10);              //wait for interrupt
      inportb(Base+4);
      restoreirq(IRQ);
      delay(100);
    }
  }else{
     sendEOI();
     initirq(IRQ,setai);
     aiflag = 1;               //set aiflag to 1 because initirq generates interrupt
     sendEOI();
     inportb(Base+4);          //clear interrupts
     outport(Base+2,0);        //channel 0, +/-10V
     outport(Base+3,0);        //remove later
     outportb(Base+4,5);        //start conversion on out 2, enable interrupts
     CtrMode(Base+8,1,2);
     CtrLoad(Base+8,1,0x2);
     CtrMode(Base+8,2,2);
     CtrLoad(Base+8,2,0xFF);
     delay(10);              //wait for interrupt
     inportb(Base+4);
     restoreirq(IRQ);
  }
  getch();
  return aiflag;
}

unsigned int aiexternal(unsigned int Base,unsigned int IRQ,int df)
{
  unsigned int test = 0;
  IRQBASE = Base;
  if(df){
    gotoxy(1,wherey()+1);
    puts("External Start Conversion Test");
    //puts("Connect a scope to pin 13. Press any key to exit.");
    while(!kbhit()){
      sendEOI();
      initirq(IRQ,setai);
      aiflag = 1;               //set aiflag to 1 because initirq generates interrupt
      sendEOI();
      inportb(Base+4);          //clear interrupts
      outport(Base+2,0);        //channel 0, +/-10V
      outport(Base+3,0);        //remove later
      outportb(Base+4,5);       //start conversion on out 2, enable interrupts
      CtrMode(Base+8,1,2);
      CtrLoad(Base+8,1,0x2);
      CtrMode(Base+8,2,2);
      CtrLoad(Base+8,2,0xFF);
      delay(10);                //wait for interrupt, maybe
      aiflag = 1;
      inportb(Base+4);          //clear interrupt from out 2 conversion
      outportb(Base+4,0x84);    //enable external start conversions,need external source???
      delay(1);                 //wait for EOC
      outportb(Base+4,0x00);
      inportb(Base+4);
      restoreirq(IRQ);
      delay(100);
    }
  }else{
    sendEOI();
    initirq(IRQ,setai);
    aiflag = 1;               //set aiflag to 1 because initirq generates interrupt
    sendEOI();
    inportb(Base+4);          //clear interrupts
    outport(Base+2,0);        //channel 0, +/-10V
    outport(Base+3,0);        //remove later
    outportb(Base+4,5);       //start conversion on out 2, enable interrupts
    CtrMode(Base+8,1,2);
    CtrLoad(Base+8,1,0x2);
    CtrMode(Base+8,2,2);
    CtrLoad(Base+8,2,0xFF);
    delay(10);                //wait for interrupt, maybe
    aiflag = 1;
    inportb(Base+4);          //clear interrupt from out 2 conversion
    outportb(Base+4,0x84);    //enable external start conversions,need external source???
    delay(1);                 //wait for EOC
    outportb(Base+4,0x00);
    inportb(Base+4);
    restoreirq(IRQ);
  }
  getch();
  return aiflag;
}

#pragma argsused
unsigned int ainotIRQ(unsigned int Base,unsigned int IRQ,int df)
{
  //no debug yet

  outport(Base+4,0);     //disable interrupts from previous tests
  IRQBASE = Base;
  sendEOI();
  aiflag = 1;            //set to zero in previous test
  initirq(IRQ,setai);
  sendEOI();
  inportb(Base+4);       //clear interrupts
  outport(Base+2,0);     //channel 0, +/-10V
  outport(Base+3,0);     //remove later
  outportb(Base+4,1);    //start conversion on out 2, do not enable interrupts
  CtrMode(Base+8,1,2);
  CtrLoad(Base+8,1,0x2);
  CtrMode(Base+8,2,2);
  CtrLoad(Base+8,2,0xFF);
  delay(10);             //wait for interrupt, maybe
  inportb(Base+4);
  restoreirq(IRQ);
  return !(aiflag);      //aiflag set to one initially, so test fails

}
