#include<graphics.h>
#include<stdlib.h>
#include<stdio.h>
#include<dos.h>
#include<conio.h>
#define uint unsigned int
#define TRUE 1
#define FALSE 0
#define NUMTIMES 33
#define FIVEU 0      //five volts unip
#define TWOU 1       //2.5 volts unip
#define TENU 2       //ten unip
#define FIVEB 4     //five bip
#define TWOB 5      //2.5 bip
#define TENB 6       //ten bip
#define RANGETWO  2
#define RANGEFIVE 4
#define RANGETEN  8
#define RANGETWENTY 16
#define SPANTWO 2.5
#define SPANFIVE 5.0
#define SPANTEN 10.0
#define SPANTWENTY 20.0
#define CALFULL 0x60        //assigned cal values to current mode
#define CALZERO 0x10        //based on other cards
#define CHECKFORBUSY(x) (inportb(x+2) & 0x80)
unsigned int eebase;
unsigned short test[7][32];//range and channel
void SwitchDisplay(int n)
{
  gotoxy(52,7);
  puts("SWITCH SETTINGS");
  gotoxy(50,8);
  puts(" 1\t 2\t 3");
  gotoxy(50,9);
  if((n & 4)==4)
    printf("ON\t");
  else
    printf("OFF\t");
  if((n & 2)==2)
    printf("ON\t");
  else
    printf("OFF\t");
  if((n & 1)==1)
    printf("ON\t");
  else
    printf("OFF\t\n");
}

double GetADData(uint adbase,uint chan)      //double
{ int index;
  double temp;
  unsigned long timeout = 655350L;
  temp=0.0L;
  outp(adbase+2, chan);     // write channel for A1216E
  delay(1);
  outportb(adbase+3,0);        // start conversion on A1216E
  delay(1);
  for(index = 0; index < NUMTIMES; index++){
    timeout = 655350L;  //
    while (CHECKFORBUSY(adbase) && (timeout > 0)) timeout--;
    if (timeout == 0L){
      printf("A/D timeout");index=NUMTIMES;
    }else
      clreol();
    temp+= inport(adbase+6);//read data from A1216E bipolar always
    outportb(adbase+3,0);        // start conversion on A1216E
  }
  temp/=NUMTIMES;//zero data
  temp-=0x8000;    //goes negative--unsigned ???????
  return temp;
}

#pragma argsused
void Calibrate(uint base,uint ebase,double sp,uint range,uint di,uint ch)
{
  double data[16];
  double data2[16];
  double zero[16];
  double span[16];
  int x = 0;
  char key;
  unsigned int chan,address,timeout,index;
  address = 0x1A0;
  outp(address+2,0); //A1216E setup
  outp(address+0,4); //A1216E setup

  gotoxy(1,7);
  puts("Calibrating                ");
  puts("Press Y to store new calibration values, any other key to continue.");
  while(!kbhit()){
    for (chan = 0; chan < ch; chan++) {   //calibrate zero
      if((range==2)||(range==5))
        outport(base+(chan*2),195);     //zero output for 0-10V and +/-10V ranges
      else
        outport(base+(chan*2),0x0);     //zero output for other ranges
      data[chan]=GetADData(address,chan);
      if(range < 3){
        if(range==2)
          zero[chan] = 195-(data[chan]/di);      //returns a positive number
        else
          zero[chan] = -(data[chan]/di);      //returns a positive number
      }else{
        if(range==5)
          zero[chan] = 195-(data[chan]/di)-2048;      //returns a positive number
        else
          zero[chan] = (-(data[chan]/di)-2048);      //returns a positive number
      }
    }//END FOR

    delay(1);

    for (chan = 0; chan < ch; chan++){    //calibrate FSV
      data2[chan]=0.0; //0.0
      if((range==5)||(range==2))
        outport(base+(chan*2),3900); //output on PCI-DA12-16
      else
        outport(base+(chan*2),0x0FFF);
      data2[chan]=GetADData(address,chan);
      if(range==2)
        span[chan] = 3900.0-((data2[chan])/di);
      else{
        if(range < 3)
          span[chan] = 4095.0-((data2[chan])/di);//unipolar
        else{
          if(range==5)
            span[chan] = 3900.0-((data2[chan])/di)-2048;//bipolar
          else
            span[chan] = 4095.0-((data2[chan])/di)-2048;//bipolar
        }
      }
    }//END FOR

    delay(1);

    for(chan = 0; chan < ch; chan++){    //print cal onscreen
      gotoxy(1,9+chan);

      if((abs(span[chan]) > 127) || (abs(zero[chan]) > 127))
        textcolor(RED);
      else
        textcolor(LIGHTGRAY);
      cprintf("Ch: %2hu   Avg. 0:%c%3x  Avg. FSV:%c%3x   Zero:% 3x    Span: % 3x ", chan,(data[chan]!=abs(data[chan]))?'-':' ',abs((int)(data[chan]/16)),(data[chan]!=abs(data2[chan]))?'-':' ',abs((int)(data2[chan]/16)),(int)(zero[chan]),(int)(span[chan]));
    } //END FOR
    textcolor(LIGHTGRAY);
    delay(100);
  }//end while

  key=getche();
  if((key=='Y') || (key=='y')){
    for(chan=0; chan< ch; chan++){
      delay(10);
      outportb(ebase+(chan*2)+(range*32),zero[chan]);//write zero offset to eeprom
      delay(10);
      outportb((ebase+(chan*2)+(range*32)+1),-span[chan]);//- write span offset to eeprom
      delay(10);
    }
  }

  for(chan=0;chan<ch;chan++){          //read values from eeprom for later
    test[range][x]=inportb(ebase+(chan*2)+(range*32));
    delay(10);
    test[range][x+1]=inportb(ebase+(chan*2)+(range*32)+1);
    delay(10);
    x+=2;
  }
}

unsigned int Cal(unsigned int InValue, int Ch, unsigned int r)
{
    unsigned int OutValue;
    unsigned short a, b;
    a = inportb(eebase + (Ch * 2) + (r * 32) + 1);
    b = inportb(eebase + (Ch * 2) + (r * 32));
    OutValue = (((4095.0 - a-b) / 4095.0) * InValue + b);
    if (OutValue > 4095)
       OutValue = OutValue - 1 + 1;
    return OutValue;
}

void debug(uint base,uint ch)
{
  uint range=0;
  uint r=0;
  uint timeout;
  uint scale;
  uint done = 0;
  char volts = '0';
  char oldvolts = '0';
  char *str;
  uint output = 0;
  float display = 0.0;
  int channel=0;
  int bipolar;
  char string[6];

  clrscr();
  inportb(base+0xF); //turn off restrict voltage
  inportb(base+0x0a);//turn off simultaneous mode
  textcolor(LIGHTGRAY);
  cprintf("Select the range and output. Press q to quit.\n\r");
  cprintf("Output values are not calibrated.\n\r");
  str = "+/-5 Volt"; //default range so it doesn't print junk at first
  range = SPANTEN; //default range
  while(!done){
  gotoxy(1,8);
 // cprintf("\nSelect the range and output. Press q to quit.\n\r");
  cprintf("RANGE         OUTPUT\n\r");
  cprintf("A.   0-5V       '1' 0 counts  \n\r");//zero volts
  cprintf("B.   0-2.5V     '2' 0x3FF counts\n\r");//25 percent
  cprintf("C.   0-10V      '3' 0x555 counts\n\r");//33 percent
  cprintf("D.   +/-5V      '4' 0x7FF counts\n\r");//50 percent
  cprintf("E.   +/-2.5V    '5' 0x800 counts\n\r");//50 percent
  cprintf("F.   +/-10V     '6' 0xAAA counts\n\r");//66 percent
  cprintf("                '7' 0xBFF counts\n\r");//75 percent
  cprintf("                '8' 0xFFF counts\n\r");//full scale
  cprintf("                '9' user defined value\n\r");
  cprintf("\nChange the range and output at any time by typing the appropriate\n\r");
  cprintf("key and moving the switch positions on the card.\n\r");
  if(kbhit())
    volts = getch();
    if(volts!=oldvolts){ //only do switch if different range/output selected
       switch(volts){
          case 'A': case 'a':  range=SPANFIVE; r=0; str = "0-5 Volt"; bipolar = FALSE;break;//4uni  0-5
          case 'B': case 'b':  range=SPANTWO; r=1; str = "0-2.5 Volt"; bipolar = FALSE;break;//8uni   0-2.5
          case 'C': case 'c':  range=SPANTEN; r=2; str = "0-10 Volt"; bipolar = FALSE;break;//2uni   0-10
          case 'D': case 'd':  range=SPANTEN; r=3; str = "+/-5 Volt"; bipolar = TRUE;break;//2bi    +/-5
          case 'E': case 'e':  range=SPANFIVE; r=4; str = "+/-2.5 Volt"; bipolar = TRUE;break;//4bi   +/-2.5
          case 'F': case 'f':  range=SPANTWENTY; r=5; str = "+/-10 Volt"; bipolar = TRUE;break;//1bi +/-10
          case 'Q': case 'q':  done = 1;break; //exit loop
          case '1': output = 0; break;
          case '8': output = 0xFFF; break;  //full scale
          case '2': output = 0x3FF; break;  //1024
          case '3': output = 0x555; break;  //1365
          case '4': output = 0x7FF; break;  //2048
          case '5': output = 0x800; break;  //2048
          case '6': output = 0xAAA; break;  //2730
          case '7': output = 0xBFF; break;  //2986
          case '9': gotoxy(40,17);
                    textcolor(LIGHTBLUE);
                    cprintf("Counts? (hex):");
                    string[0]=5;
                    cgets(string);
                    sscanf(string+2,"%3x",&output);
                  //  output=Cal(output,channel,r);
                    volts = 0;
                    gotoxy(1,21);
                    textcolor(LIGHTGRAY);
                    break;
       }
    }

    oldvolts = volts;
    if(bipolar)
      display = (output * (range/4095.0))-range*0.5;//convert to volts
    else
      display = (output * (range/4095.0));//convert to volts

    for(channel = 0; channel < ch; channel++)
      outport(base+(channel*2),output);     //output on PCI-DA12-16
    delay(10);
    cprintf("\nValue being output on all channels.\n\r");
    //make counts different color
    textcolor(LIGHTBLUE);
    cprintf("\n%3x  counts    ",(int)(output));
    textcolor(LIGHTGRAY);
    cprintf("%2.3f volts    %10s  range  \n\r",display,str);
  }//end while
}

void Current(uint base, uint ebase,uint ch)
{
  int channel;
  int r = 6;  //sixth entry in eeprom
  puts("Connect a meter to the current jumper.");
  puts("Outputting zero on all channels...");
  puts("Press any key to output 7FF.");
  inportb(base+0xF);    //clear restrict-output-voltage
  for(channel = 0; channel < ch; channel++){
    delay(10);
    outportb(ebase+(channel*2)+(r*32),CALZERO);//write zero offset to eeprom
    delay(10);
    outportb((ebase+(channel*2)+(r*32)+1),CALFULL);//write span offset to eeprom
    delay(10);
  }
  //write some numbers to eeprom
    for(channel = 0; channel < ch; channel++)
      outport(base+(channel*2),0);//output
  while(!kbhit());
  getch();
  puts("Outputting 7FF counts. Press any key to output FFF counts ");
    for(channel = 0; channel < ch; channel++)
      outport(base+(channel*2),0x7FF);//output
  while(!kbhit());
  getch();
  puts("Outputting FFF on all channels...");
  puts("Press any key to continue calibration.");
  for(channel = 0; channel < ch; channel++)
    outport(base+(channel*2),0xFFF);//output
  while(!kbhit());
  getch();
}


int dac(uint base, uint ebase, uint ch, int df)
{
  unsigned int timeout = 65535;
  int i,x,t;
  textcolor(LIGHTGRAY);
  eebase = ebase;
  if(!df){
    gotoxy(1,wherey()+1);
    cprintf("A1216E SETUP:  ");
    cprintf("Base address: 1A0; Range: +/-10V.\n\r");
    cprintf("PCI-DA12-16 SETUP\n\r");
    cprintf("--Set all channels to 4-20mA range (0-5V).\n\r");
    inportb(base+0xF); //dac setup, clear restrict-output-voltage
    inportb(base+0x2); //dac setup, release simultaneous update mode//redundant
    inportb(base+0xA); //dac setup, update and release from simul mode
    SwitchDisplay(FIVEU);//current range is 0-5 volts

    Current(base,ebase,ch);//check output for current
    clrscr();
   // cprintf("--Set all channels to 0-5V range.\n\r"); //already there
    //SwitchDisplay(FIVEU);

    Calibrate(base,ebase,SPANFIVE,0,RANGEFIVE,ch);//5 uni
    clrscr();
    cprintf("A1216E SETUP:  Base address: 1A0; Range: +/-10V\n\r");
    cprintf("--Set all DAC channels to 0-2.5V range. (See switch settings below.)\n\r");
    SwitchDisplay(TWOU);
    Calibrate(base,ebase,SPANTWO,1,RANGETWO,ch);//2.5 uni
    clrscr();
    cprintf("A1216E SETUP:  Base address: 1A0; Range: +/-10V\n\r");
    cprintf("--Set all DAC channels to 0-10V range. (See switch settings below.)\n\r");
    SwitchDisplay(TENU);
    Calibrate(base,ebase,SPANTEN,2,RANGETEN,ch);//10 uni
    clrscr();
    cprintf("A1216E SETUP:  Base address: 1A0; Range: +/-10V\n\r");
    cprintf("--Set all DAC channels to +/-10V range. (See switch settings below.)\n\r");
    SwitchDisplay(TENB);
    Calibrate(base,ebase,SPANTWENTY,5,RANGETWENTY,ch);//10 bi

    clrscr();
    cprintf("A1216E SETUP:  Base address: 1A0; Range: +/-10V\n\r");
    cprintf("--Set all DAC channels to +/-2.5V range. (See switch settings below.)\n\r");
    SwitchDisplay(TWOB);
    Calibrate(base,ebase,SPANFIVE,4,RANGEFIVE,ch);//2.5 bi
    clrscr();

    cprintf("A1216E SETUP:  Base address: 1A0; Range: +/-10V\n\r");
    cprintf("--Set all DAC channels to +/-5V range. (See switch settings below.)\n\r");
    SwitchDisplay(FIVEB);
    Calibrate(base,ebase,SPANTEN,3,RANGETEN,ch);//5 bi

    clrscr();
    puts("Values written to EEPROM\n");
    t=0;
    for(i = 0; i < 6; i++){  //7

  //    if(i!=3){                     //no range=3
        gotoxy(t*12+8,2);
        cprintf("RANGE %i",i);
        gotoxy(t*12+8,3);
        cprintf("SPAN ZERO");
        for(x = 0; x < 32; x+=2){
          if(i==0){
            gotoxy(1,(x/2)+4);
            cprintf("Ch:%2i",(int)(x/2));
          }
          gotoxy(t*12+8,(x/2)+4);
          cprintf("%c%3x :%c%3x",(test[i][x+1]!=abs(test[i][x+1])?'-':' '),
          abs(test[i][x+1]),(test[i][x]!=abs(test[i][x])?'-':' '),
          abs(test[i][x]));
        }//end for x
      t++;
  //    }//end if
    }//end for i
    puts("");  //output values, bipolar vs. uni
   // for(i = 0; i < 16; i++){           //where is card told range number??
   //   outport((eebase + ((i * 2) + (inportb(eebase + 240 + i) * 32))),5);
   //   outport((eebase + ((i * 2) + (inportb(eebase + 240 + i) * 32)) + 1),5);
   // }
  }else{       //have to run non-debug before debug or val. not calibrated
    textcolor(LIGHTGRAY);
    debug(base,ch);
  }
  return 0;
}

