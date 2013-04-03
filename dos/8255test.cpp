#include <dos.h>
#include <conio.h>
#include <stdio.h>
#include <stdlib.h>
#include "irq0-15.h"

#define SLOWNESS 200

unsigned int isr_flag;  /* used to indicate an interrupt occurred */
unsigned int baddr;

#pragma argsused
unsigned _8255TstPCIA1216A(unsigned int BaseA, int df)
{
  unsigned int result=0;
  static int test = 0;
  int val,val2;
  outportb(BaseA + 3,0x8B);              // A output only
  outportb(BaseA,0xAA);                  // write AA to A
  outportb(BaseA + 3, 0x9B);             // set all inputs
  delay(10);                              // wait
  result+=(inportb(BaseA)^0xAA);        // read value on A
  outportb(BaseA + 3,0x92);              // C output only
  outportb(BaseA + 2,0xA5);              // write A5 to C
  delay(10);                             // wait
  result+=(inportb(BaseA+2)^0xA5);      // read value on C
  outportb(BaseA+4,0x1B);              // clear tri-state
  outportb(BaseA,0xFF);                  //write FF to all
  outportb(BaseA+1,0xFF);                //write FF to all
  outportb(BaseA+2,0xFF);                //write FF to all
  outportb(BaseA + 3, 0x9B);             //set input on all
  if(df)
  {
    gotoxy(1,wherey()+1);
    textcolor(LIGHTGRAY);
    cprintf("Port %d\n\r",test);
    cprintf("Value wrote to A: AA\n\r");
    cprintf("Value read on A: \n\r");
    cprintf("Value wrote to C: A5\n\r");
    cprintf("Value read on C:\n\r");
    test++;
    gotoxy(19,wherey()-3);
    do
    {
      textcolor((val==0xAA)?GREEN:RED);
      cprintf("%x",val);
      gotoxy(19,wherey()+2);
      textcolor((val2==0xA5)?GREEN:RED);
      cprintf("%x",val2);
      gotoxy(19,wherey()-2);
    }while(!kbhit());
    getch();
    printf("\n\n\n");
  }
  return result;
}

#pragma argsused
unsigned _8255Tst(unsigned int BaseA, int df)
{
  unsigned int result=0;
  static int test = 0;
  int val,val2;
  outportb((BaseA&(~0xf))+0xB,0xFF); //disable cos
  outportb(BaseA + 3,0x8B);              // A output only
  outportb(BaseA,0xAA);                  // write AA to A
  outportb(BaseA + 3, 0x9B);             // set all inputs
  delay(10);                              // wait
  result+=(inportb(BaseA)^0xAA);        // read value on A
  val=(inportb(BaseA));            //for debug flag
  outportb(BaseA + 3,0x92);              // C output only
  outportb(BaseA + 2,0xA5);              // write A5 to C
  delay(10);                             // wait
  result+=(inportb(BaseA+2)^0xA5);      // read value on C
  val2=(inportb(BaseA + 2));    //for debug flag
  outportb(BaseA+3,0x1B);              // clear tri-state
  outportb(BaseA,0xFF);                  //write FF to all
  outportb(BaseA+1,0xFF);                //write FF to all
  outportb(BaseA+2,0xFF);                //write FF to all
  outportb(BaseA + 3, 0x9B);             //set input on all
  if(df)
  {
    gotoxy(1,wherey()+1);
    textcolor(LIGHTGRAY);
    cprintf("Port %d\n\r",test);
    cprintf("Value wrote to A: AA\n\r");
    cprintf("Value read on A: \n\r");
    cprintf("Value wrote to C: A5\n\r");
    cprintf("Value read on C:\n\r");
    test++;
    gotoxy(19,wherey()-3);
    do
    {
      textcolor((val==0xAA)?GREEN:RED);
      cprintf("%x",val);
      gotoxy(19,wherey()+2);
      textcolor((val2==0xA5)?GREEN:RED);
      cprintf("%x",val2);
      gotoxy(19,wherey()-2);
    }while(!kbhit());
    getch();
    printf("\n\n\n");
  }
  return result;
}


void interrupt setflag(...)
{
  isr_flag = 1;                /* indicate an interrupt has occurred */
  outportb(baddr + 2,0x00);    // turn off irq source for DIO-120
  outportb(baddr|0x1F,0x00); // clear interrupt for DIO-120
  //outportb(baddr|0xF,0x00);  // turn off irq source for DIO-48S and dio-24
  inportb(baddr);
}

void interrupt COSsetflag(...)
{
  isr_flag = 1;               /* indicate an interrupt has occurred */
  outportb(baddr|0xF,0x00);  //turn off irq source for IOD-48S and dio-24
  inportb(baddr);
}


#pragma argsused
unsigned _8255IENTest(unsigned Base,unsigned IRQ, int df, int D48Flag,int COSFlag,int hdflag)
{
  int result2=0;
  static int test2 = 0;
  baddr=Base;
  if(!df){
    textcolor(LIGHTGRAY);
    cprintf("Install the IEN jumper for port %d.\n\r",test2);
    test2++;
    cprintf("Press any key to continue.\n\r");
    while(!kbhit());
    getch();
  }
  outportb(Base|0x1F,0x00); //enable interrupts on DIO-120
  if(D48Flag)
    outportb(Base|0xF,0x00); //turn off interrupt for DIO-48

  if(COSFlag)
  {
    outportb((Base&0xfff0)|0xB,0xFF); //disable cos
    outportb(Base|0xF,0x00); //turn off interrupt for IOD-48S
  }

  if(hdflag)
    outportb(Base+0xE,0x00); //enable interrupt for dio-24
  outportb(Base + 3,0x81);  //Port CLo Input, rest outputs, change to 88 for CHi input
  outportb(Base + 2,0x00);
  initirq(IRQ,setflag);
  delay(10);
  //if(hdflag)
  //  outportb(Base+0xF,0x00);//clear interrupt latch for dio-24
  //clear pending interrupts, program can run more than once
  outportb(0x20,0x20);
  outportb(0xA0,0x20);
  isr_flag = 0;

  outportb(Base + 2,0x00);  //turn off irq source for IOD-120
  outportb(Base + 2,0x80);  //generate irq via card pin C7, change to 08 for C3
  delay(10);
  outportb(Base + 2,0x00);  //turn off irq source for IOD-120
  if(D48Flag)
    outportb(Base|0xF,0x00);  //turn off irq source for DIO-48

  if(COSFlag)
    outportb(Base|0xF,0x00);  //turn off irq source for IOD-48S

  if(hdflag)
    outportb(Base+0xD,0x00);//turn off interrupt for dio-24
  outportb(0x20,0x20);
  outportb(0xA0,0x20);

  restoreirq(IRQ);
  outportb(Base|0x1E, 0x00); //disable interrupts on DIO-120
  outportb(Base + 2,0x00);
  if(D48Flag)
    outportb(Base|0xF,0x00); //turn off interrupt for IOD-48S

  if(COSFlag)
    outportb(Base|0xF,0x00); //turn off interrupt for IOD-48S

  if(hdflag)
    outportb(Base+0xF,0x00);//clear interrupt latch for dio-24


  if(df)
  {
    textcolor(LIGHTGRAY);
    cprintf("Install the IEN jumper for port %d.\n\r",test2);
    cprintf("Press any key to exit IRQ debug.\n\r");
    cprintf("Port %d\n\r",test2);
    test2++;
    outportb(Base|0x1F,0x00); //enable interrupts for DIO-120
    delay(200);
    do
    {
      delay(100);
      initirq(IRQ,setflag);
      delay(10);
      sendEOI();
      isr_flag=0;

      if(hdflag)
      {
        outportb(Base+0xE,0x00); //enable interrupt for dio-24
        outportb(Base+0xF,0x00);//clear interrupt latch for dio-24
      }
      outportb(Base + 2,0x00);  //turn off irq source for IOD-120
      outportb(Base + 2,0x80);  //generate irq via card pin C7, change to 08 for C3
      delay(10);
      outportb(Base + 2,0x00);  //turn off irq source for IOD-120

      if(D48Flag)
        outportb(Base|0xF,0x00);  //turn off irq source for IOD-48S

      if(COSFlag)
        outportb(Base|0xF,0x00);  //turn off irq source for IOD-48S

      if(hdflag)
      {
        outportb(Base+0xD,0x00);//turn off interrupt for dio-24
        outportb(Base+0xF,0x00);//clear interrupt latch for dio-24
      }

      sendEOI();
      restoreirq(IRQ);
      result2 = isr_flag;
      textcolor(result2?GREEN:RED);
      cprintf("%s",result2?"IRQ    ":"NoIRQ    ");
      if((80 - wherex()) < 10)
      gotoxy(1,wherey());
      result2 = 0;
    }while(!kbhit()&&df);
    getch();
    printf("\n");
  }
  outportb(Base+3,0x9b);//put card back in all input mode
  outportb(Base|0x1E,0x00); //disable interrupts for DIO-120
  if(D48Flag)
    outportb(Base|0xF,0x00); //turn off interrupt for IOD-48S
  if(COSFlag)
    outportb(Base|0xF,0x00); //turn off interrupt for IOD-48S
  if(hdflag)
    outportb(Base + 0xF,0x00);//clear interrupt latch for dio-24

  return isr_flag?0:1;

}

#pragma argsused
unsigned _8255notIENTest(unsigned int Base,unsigned int IRQ, int df, int hdflag)
{
  baddr=Base;
  outportb((Base&0xfff0)|0xB,0xFF); //disable cos in case its a 48S
  outportb(Base|0xF,0x00); //turn off interrupt for IOD-48S
  if(hdflag)
  {
    outportb(Base+0xE,0x00); //enable interrupt for dio-24
    outportb(Base+0xF,0x00);//clear interrupt latch for dio-24
  }
  outportb(Base + 3,0x81);  //Port CHi Input, rest outputs
  outportb(Base + 2,0x00);
  initirq(IRQ,setflag);

  //clear pending interrupts, program can run more than once
  outportb(0x20,0x20);
  outportb(0xA0,0x20);
  isr_flag = 0;

  //outportb(Base + 2,0x80);  //generate irq via card pin C7
  delay(10);
  outportb(Base + 2,0x00);  //turn off irq source
  if(hdflag)
  {
    outportb(Base + 0xD,0x00);//turn off irq for dio-24
    outportb(Base+0xF,0x00);//clear interrupt latch for dio-24
  }
  outportb(0x20,0x20);
  outportb(0xA0,0x20);

  restoreirq(IRQ);
  outportb(Base + 2,0x00);  //turn off irq source
  if(hdflag)
    outportb(Base+0xF,0x00);//clear interrupt latch for dio-24
  outportb(Base+3,0x9B);    //put card back in all input mode

  return isr_flag?1:0;
}

#pragma argsused
unsigned _8255WalkingTest(unsigned Base, int DAC, int df)
{
  int index,dis;

  unsigned char value;
  static int test3 = 0;
  //unsigned char value[2];
  unsigned result = 0;
  outportb(Base + 3, 0x82);
  if(DAC)                  //for DAC 8255, second write with msb = 0
    outportb(Base + 3, 0x02);
  if(df)
  {
    printf("\n");
    gotoxy(1,wherey());
    textcolor(LIGHTGRAY);
    cprintf("Port %d\n\r",test3);
    test3++;
    gotoxy(10,wherey());
    cprintf(" Value out A     Value in B\n");
    do
    {
      for(index = 0; index < 8; index++)
      {
        outportb(Base+0,1<<index);     /* write value to port a              */
        delay(30);
        value = inportb(Base + 1);  //read port B
        delay(50);
        for (dis = 0;dis < 8;dis++)
        {
          gotoxy(20-dis,wherey());
          textcolor(GREEN);
          putch(((inportb(Base+0))&(1<<dis))?'1':'0');
          gotoxy(35-dis,wherey());
          textcolor( (value == (1 << index)) ? GREEN : RED );
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
    for(index = 0; index < 8; index++)
    {
      outportb(Base+0,1<<index);     /* write value to port a              */
      delay(30);
      value = inportb(Base + 1);  //read port B
      if (value != 1<<index)
         result++;
    }
  }
  index = 0;
  dis = 0;
  outportb(Base + 3, 0x90);
  if(DAC)
    outportb(Base + 3, 0x10);  //DAC 8255 write
  if(df)
  {
    printf("\n");
    gotoxy(10,wherey());
    textcolor(LIGHTGRAY);
    cprintf(" Value out B     Value in A\n");
    do{
      for(index = 0; index < 8; index++)
      {
        outportb(Base+1,1<<index);     /* write value to port b              */
        delay(50);
        value = inportb(Base);  //read port A
        delay(10);
        for (dis = 0;dis < 8;dis++)
        {
          gotoxy(20-dis,wherey());
          textcolor(GREEN);
          putch(((inportb(Base+1))&(1<<dis))?'1':'0');
          gotoxy(35-dis,wherey());
          textcolor((value==(1<<index))?GREEN:RED);
          putch((value&(1<<dis))?'1':'0');

        }
        if (value != 1<<index)
          result++
        ;
      }
    }while(!kbhit()&&df);
    getch();
    printf("\n");
  }
  if(!df)
  {
    for(index = 0; index < 8; index++)
    {
      outportb(Base+1,1<<index);     /* write value to port b              */
      delay(10);
      value = inportb(Base + 0);  //read port a
      if (value != 1<<index)
         result++;
    }
  }

  index = 0;
  outportb(Base + 3, 0x88);
  if(DAC)
    outportb(Base + 3, 0x08);   //DAC write
  if(df)
  {
    printf("\n");
    gotoxy(10,wherey());
    textcolor(LIGHTGRAY);
    cprintf(" Value out Clo     Value in Chi\n");
    do
    {
      for(index = 0; index < 4; index++)
      {
        outportb(Base + 2, 1<<index);//write value to port c low
        delay(200);
        value = inportb(Base + 2) >> 4;  //read port c high
        delay(10);
        for (dis = 0;dis < 4;dis++)
        {
          gotoxy(20-dis,wherey());
          textcolor(GREEN);

          putch(((inportb(Base+2)) & (1 << dis))?'1':'0');

          gotoxy(35-dis,wherey());
          textcolor((value==(1<<index))?GREEN:RED);
          putch((value&(1<<dis))?'1':'0');
        }
        if (value != 1<<index)
           result++
        ;
      }
    }while(!kbhit()&&df);
    getch();
    printf("\n");
  }
  if(!df)
  {
    for(index = 0; index < 4; index++)
    {
      delay(10);
      outportb(Base+2,1<<index);     /* write value to port clo              */
      delay(10);
      value = inportb(Base + 2)/16;  //read port chi
      if (value != 1<<index)
         result++;
    }
  }

  index = 0;
  outportb(Base + 3, 0x81);
  if(DAC)
    outportb(Base + 3, 0x01);   //DAC write
  if(df)
  {
    printf("\n");
    gotoxy(10,wherey());
    textcolor(LIGHTGRAY);
    cprintf(" Value out Chi     Value in Clo\n");
    do
    {
      for(index = 0; index < 4; index++)
      {
        delay(200);
        outportb(Base+2,16<<index);     /* write value to port chi              */
        delay(10);
        value = inportb(Base + 2) << 4;  //read port c lo
        for (dis = 0;dis < 4;dis++)
        {
          gotoxy(20-dis,wherey());
          textcolor(GREEN);
          putch(((inportb(Base+2))&(1<<dis+4))?'1':'0');
          gotoxy(35-dis,wherey());
          textcolor((value==(16<<index))?GREEN:RED);
          putch((value&(16<<(dis)))?'1':'0');
        }
        if (value != 16<<index)
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
      delay(10);
      outportb(Base+2,16<<index);     /* write value to port chi              */
      delay(10);
      value = inportb(Base + 2) << 4;  //read port c lo
      if (value != 16<<index)
         result++;
    }
  }
  return result;
}

#pragma argsused
unsigned _8255COSDebug(unsigned int Base,unsigned int IRQ,int df)
{
   unsigned test = 0x01;
   initirq(IRQ,COSsetflag);

   outportb(Base+0x03,0x8B); //set
   do
   {
       outportb((Base & 0xFFF0) + 0xB, 0xF6 | (((Base & 4) == 0) ? 0x08 : 0x01));     //enable interrupt on port A, group 0 and 1
       outportb(Base + 0x0f, 0);
       sendEOI();
       isr_flag = 0;
       outportb(Base, 0x00);
       //delay(10);
       outportb(Base, test);
       delay(1);
       //geninterrupt(IRQ+8);
       printf("%d", isr_flag);
       outportb(Base + 0x0B, 0xff);
       outportb(Base + 0x0F, 0);
   }while(!kbhit());

   restoreirq(IRQ);
   return isr_flag ? 0 : 1;
}

#pragma argsused
unsigned _8255COS(unsigned int Base,unsigned int IRQ,int df)
{
  unsigned resval=0;
  int i,y,dis,dir,zzz,dndSLOWNESS;
  int index = 0;
  int test = 0;
  int inc = 1;
  static int port = 0;
  baddr=Base;

  //_8255COSDebug(Base,IRQ,df); //debug, duh

  dndSLOWNESS=df?SLOWNESS:10;
//  outportb(Base + 0x0f, 0);

  if(df)
  {
    textcolor(LIGHTGRAY);
    cprintf("Press any key to exit COS debug\n\r");
    cprintf("Port %d\n\r",port);
    cprintf("           Value out A\n\r");
    cprintf("\n\r");
    y = 2;
    gotoxy(20,wherey()-1);
  }

  outportb((Base&0xFFF0)+0xB, 0xFF);//disable cos interrupt for all ports
  outportb(Base + 3, 0x8B);     //set port B, C to input, A output
  outportb(Base+0,0);
  initirq(IRQ,COSsetflag);//initialize IRQ
  delay(10);
  test=0x00;
  dir=0;
  outportb((Base&0xFFF0)+0xB, 0xED | (((Base&4)==0) ? 0x10 : 0x02)); //enable interrupt on port B, group 0 and 1

  sendEOI(); //send end-of-interrupt
  for(zzz=0;zzz<2;zzz++) for(i = 0; (i<0x8); i++)
  {
    if (i==0)
      dir=(dir==0)?1:0;
    isr_flag = 0;
    delay(30);

    test=(dir==0)? test & (~(1<<i)) : test|(1<<i);

    outportb(Base + 0, test);//generate interrupt on write to port A
    delay(dndSLOWNESS);//slow enough to see bits change
    outportb(Base|0xF,0);   //clear interrupt
    resval += isr_flag?0:1; //increment resval for return
    sendEOI();
    dis=i;

    if(df)
    {
      gotoxy(20-dis,wherey());
      textcolor((isr_flag)?GREEN:RED);
      putch(((test)&(1<<dis))?'1':'0');//display output from A
    }

  } //end test loop
  outportb((Base&0xFFF0)+0xB,0xFF); //disable interrupts

  if(df)
  {
    getch();
    printf("\n");
    textcolor(LIGHTGRAY);
    cprintf("          Value out B \n\r");
  }

  test=0x00;
  dir=0;

  outportb((Base&0xFFF0)+0xB, 0xFF);//disable cos interrupt for all ports
  outportb(Base + 3, 0x99);     //set port A, C to input, B output
  delay(10);
  outportb((Base&0xFFF0)+0xB, 0xF6 | (((Base&4)==0) ? 0x08 : 0x01)); //enable interrupt on port A, group 0 and 1
  sendEOI(); //send end-of-interrupt
  for(zzz=0;zzz<2;zzz++) for(i = 0; (i<0x8); i++)
  {
    if (i==0)
      dir=(dir==0)?1:0;
    isr_flag = 0;
    delay(30);


    test=(dir==0)? test & (~(1<<i)) : test|(1<<i);


    outportb(Base + 1, test);//generate interrupt on write to port B
    delay(dndSLOWNESS);//slow enough to see bits change
    outportb(Base|0xF,0);   //clear interrupt
    resval += isr_flag?0:1; //increment resval for return
    sendEOI();
    dis=i;

    if(df)
    {
      gotoxy(20-dis,wherey());
      textcolor((isr_flag)?GREEN:RED);
      putch(((test)&(1<<dis))?'1':'0');//display output from A
    }

  } //end test loop
  outportb((Base&0xFFF0)+0xB,0xFF); //disable interrupts

  if(df)
  {
    getch();
    printf("\n");
    textcolor(LIGHTGRAY);
    cprintf("          Value out Clo \n\r");
  }

  test=0x00;
  dir=0;
  outportb((Base & 0xFFF0)+0xB, 0xFF);//disable cos interrupt for all ports
  outportb(Base + 3, 0x9A);     //set port B, C to input, A output
  delay(10);
  outportb((Base&0xFFF0)+0xB, 0xDB | (((Base&4)==0) ? 0x20 : 0x04));//enable interrupt on port C, group 0 and 1
  sendEOI(); //send end-of-interrupt
  for(zzz=0;zzz<2;zzz++) for(i = 0; (i<0x4); i++)
  {
    if (i==0)
      dir=(dir==0)?1:0;
    isr_flag = 0;
    delay(30);


    test=(dir==0)? test & (~(1<<i)) : test|(1<<i);


    outportb(Base + 2, test);//generate interrupt on write to port C
    delay(dndSLOWNESS);//slow enough to see bits change
    outportb(Base|0xF,0);   //clear interrupt
    resval += isr_flag?0:1; //increment resval for return
    sendEOI();
    dis=i;

    if(df)
    {
      gotoxy(20-dis,wherey());
      textcolor((isr_flag)?GREEN:RED);
      putch(((test)&(1<<dis))?'1':'0');//display output from C
    }

  } //end test loop

  outportb((Base&0xFFF0)+0xB,0xFF); //disable interrupts

  if(df)
  {
    getch();
    printf("\n");
    textcolor(LIGHTGRAY);
    cprintf("          Value out Chi \n\r");
  }

  test=0x00;
  dir=0;
  outportb((Base&0xFFF0)+0xB, 0xFF);//disable cos interrupt for all ports
  outportb(Base + 3, 0x93);     //
  delay(10);
  outportb((Base&0xFFF0)+0xB, 0xDB| (((Base&4)==0) ? 0x20 : 0x04));//enable interrupt on port C, group 0 and 1
  sendEOI(); //send end-of-interrupt
  for(zzz=0;zzz<2;zzz++) for(i = 0; (i<0x4); i++)
  {
    if (i==0)
      dir=(dir==0)?1:0;
    isr_flag = 0;
    delay(30);


    test=(dir==0)? test & (~(1<<i)) : test|(1<<i);


    outportb(Base + 2, test<<4);//generate interrupt on write to port c
    delay(dndSLOWNESS);//slow enough to see bits change
    outportb(Base|0xF,0);   //clear interrupt
    resval += isr_flag?0:1; //increment resval for return
    sendEOI();
    dis=i;

    if(df)
    {
    gotoxy(16-dis,wherey());
    textcolor((isr_flag)?GREEN:RED);
    putch(((test)&(1<<dis))?'1':'0');//display output from C
    }

  } //end test loop
  outportb((Base&0xFFF0)+0xB,0xFF); //disable interrupts
  outportb(Base+3,0x9b);//set all ports to input to avoid collision through wrap plug

  if(df)
  {
    getch();
    printf("\n");
  }
    //reset mask and interrupt vector
  restoreirq(IRQ);
  sendEOI();
  outportb((Base&0xFFF0)+0xB,0xFF); //disable interrupts
  port++;
  return resval;
}

#pragma argsused
unsigned _8255PairPair(unsigned int Base, unsigned int IRQ, int df)
{
  int bFail = 0, bFailPass;
  for ( int GI = 0; GI <= 1; ++GI )
  {
    int G = GI * 8;

    textattr(0x07);
    cprintf("Connect PPIs %d and %d with the test jig. Press any key to continue.", GI * 2, GI * 2 + 1);
    getch();
    cputs("\r                                                                               \r");
    for ( int BI = 0; BI <= 1; ++BI )
    {
      int B = G + BI * 4;
      int nB = G + (BI ^ 1) * 4;

      bFailPass = 0;
      cprintf("(from %X to %X)\r\n", Base + B, Base + nB);
      outportb(Base + nB + 3, 0x9B);
      outportb(Base + B + 3, 0x80);
      unsigned long M, IM;
      for ( M = 1; M & 0xFFFFFF; M <<= 1 )
      {
        outport(Base + B, M);
        outportb(Base + B + 2, M >> 16);
        delay(1);
        unsigned long LoBuf;
        LoBuf = inport(Base + nB);
        IM = inportb(Base + nB + 2);
        IM <<= 16;
        IM |= LoBuf;

        if ( IM ^ M )
        {
          bFail = -1;
          bFailPass = -1;
          if ( df )
          {
            textattr(0x0C);
            cprintf("wrote %06lX, read %06lX\r\n", M, IM);
          }
        }
      }
      textattr(bFailPass ? 0x0C : 0x0A);
      cprintf("%d to %d%20s\r\n", GI * 2 + BI, GI * 2 + 1 - BI, bFailPass ? "FAIL" : "PASS");
      textattr(0x07);
      if ( bFailPass && df )
      {
        cputs("Press any key to continue.");
        getch();
        cputs("\r                                                                               \r");
      }
    }
  }

  textattr(bFail ? 0x0C : 0x0A);
  cprintf("Test%22s", bFail ? "FAIL" : "PASS");

  return bFail;
}
