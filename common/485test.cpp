#include "display.h"
#include "io.h"
#include "data_types.h"
#include "common_objects.h"
#include "irq.h"

unsigned Base[8];
int numport = 0;
unsigned IRQ2=0;
unsigned OUT;               //which port is outputting?
struct comstuff{
  unsigned base;
  unsigned char tail;  //wraps at 255   circular buffer tail
  unsigned char head;  //wraps at 255   circular buffer head
  unsigned char data[256];
  unsigned char x,y;      //cursor position
  unsigned char x1,y1,x2,y2,fc,bc;
}com[8];

// void interrupt ISR(...)
// void ISR(void)
void isr(...)
{

  int i,j,n=0;

  while ((n < numport)) {
          j = INPORTB(com[n].base + 2);
          if ((j & 0x06) == 0x04){ // if data is waiting
            com[n].data[++com[n].head] = INPORTB(com[n].base); // read & store
            com[n].head %= 256;
          }else{
            if ((j & 0x06) == 0x06){//error or other irq source
            INPORTB(com[n].base+5);//line status error
            com[n].data[++com[n].head] = ''; // read & store
            com[n].head%=256;
          }//end if data is waiting
      }//end while (n<numport)
      n++;
  } // end while
  sendEOI();
}

void setTitleWindow(void) { WINDOW(1,1,80,1);TEXTATTR((BLUE<<4)|(WHITE));}
void setSideWindow(void){ WINDOW(1,1,4,25);TEXTATTR((BLUE<<4)|(WHITE));}
void setFullWindow(void){ WINDOW(1,1,80,25);TEXTATTR((BLACK<<4)|(LIGHTGRAY));}

void setComWindow(unsigned char n)
{ WINDOW(com[n].x1,com[n].y1,com[n].x2,com[n].y2);
  TEXTBACKGROUND(com[n].bc);
  TEXT_COLOR(com[n].fc);
  GOTOXY(com[n].x,com[n].y);
}

void drawsidebar(int maxport)
{ int n=0;
  setSideWindow();
  CLRSCR();
  for (n=0;n<maxport;n++){
    GOTOXY(1,n*(24/maxport)+3);
    CPRINTF("COM%c",n+'A');
     GOTOXY(1,n*(24/maxport)+4);
    CPRINTF("DATA");
     GOTOXY(1,n*(24/maxport)+4);
    CPRINTF("%04X",com[n].base);
  }
  GOTOXY(1,OUT * (24/maxport) + 2);
  PUTS("OUT");

}


void drawscreen(int maxport)
{
  int i;
  setTitleWindow();
  CLRSCR();
  GOTOXY(5,1);
  PUTS("PCI-COM Shared IRQ Debug.  Version 1.00   F10 for help, ALT-X to eXit.");
  for (i=0;i<maxport;i++){
    setComWindow(i);CLRSCR();
  }
  setSideWindow();
  drawsidebar(maxport);
}

void drawhelp(int maxport)
{
  setFullWindow();
  TEXTATTR((BLUE<<4)|YELLOW);
  CLRSCR();
  PUTS("PCI-COM Shared IRQ Debug.\n\r");
  PUTS("\n\n\n\r");
  PUTS("Use F10 for help\n\r");
  PUTS("Use ALT-X to exit\n\r");
  PUTS("Use F1-F8 for Output COM Port selection.  F1=COMA, F2=COMB etc.\n\r");
  PUTS("\n\r");
  GETCH();
  drawscreen(maxport);

}


unsigned handledata(int maxport)
{ int n=0;char dat;
  for (n=0;n<maxport;n++){
     while (com[n].head != com[n].tail){
       setComWindow(n);
       PUTCH(dat=com[n].data[++com[n].tail]);
       if (dat==0x08) {PUTCH(' ');PUTCH(dat);}
       com[n].x=WHEREX();
       com[n].y=WHEREY();
     }
  }
  return 0;//debug -- add error conditionals
}

unsigned handlekeys(int maxport)
{ unsigned char key=0;
  if (!kbhit()) return 0;               //if no character continue program
  setComWindow(OUT);
  key=GETCH();
  if (!key){
    key=GETCH();
    switch(key){
       case 45:return 1; //ALT-X
       //case 67:geninterrupt(10+0x68);break;//IRQ+((IRQ<8)?8:0x68));break;
       case 59: // F1-F8
       case 60:
       case 61:
       case 62:
       case 63:
       case 64:
       case 65:
       case 66: OUT=(key-59) % maxport;
        drawsidebar(maxport);
        break;
       case 68: drawhelp(maxport);
    }//end switch
  }else{
     int timeout = 65535;
     do{
    timeout--;
     }while (!(((INPORTB(com[OUT].base+5) & 0x20)) || (timeout==0)));

     if (timeout == 0) {
    setComWindow(OUT);
    PUTS("Hardware transmission failure #1!");
    return 1;
     }
     OUTPORTB(com[OUT].base,key);
      //geninterrupt(IRQ2+0x68);//generates software interrupt for Dennis
     PUTCH(key);
     if (key==0x08) {PUTCH(' ');PUTCH(key);}
     com[OUT].x=WHEREX();
     com[OUT].y=WHEREY();
     DELAY(1);
//     geninterrupt(IRQ2+0x68);
  }//end else
  return 0;
}

//function for automated test, doesn't display data on screen
int handlekeys2(int maxport,int index)
{
  int timeout = 65535;
  char ch,ch1,ch2,ch3,ch4,ch5,ch6,ch7;
  int flag = 0;
  OUT=index;
  for(ch = 'A';ch <= 'Z';ch++){
    do{
    timeout--;
     }while (!(((INPORTB(com[OUT].base+5) & 0x20)) || (timeout==0)));

     if (timeout == 0) {
 // setComWindow(OUT);
    PUTS("Hardware transmission failure #1!");
    return 1;
     }
     OUTPORTB(com[OUT].base,ch);//send A through Z on com[i]
     DELAY(10);
     ch1 = INPORTB(com[(index+1)%maxport].base);
     if(ch1!=ch)
       flag++;
     if(maxport > 2){
       ch2 = INPORTB(com[(index+2)%maxport].base);
       if(ch2!=ch)
         flag++;
       ch3 = INPORTB(com[(index+3)%maxport].base);
       if(ch3!=ch)
         flag++;
       if(maxport==8){
         ch4 = INPORTB(com[(index+4)%maxport].base);
         if(ch4!=ch)
           flag++;
         ch5 = INPORTB(com[(index+5)%maxport].base);
         if(ch5!=ch)
           flag++;
         ch6 = INPORTB(com[(index+6)%maxport].base);
         if(ch6!=ch1)
           flag++;
         ch7 = INPORTB(com[(index+7)%maxport].base);
         if(ch7!=ch)
           flag++;
       }//end maxport loop for com-8s
     }//end maxport > 2
    }//end for A through Z

  return flag;
}

void initcomms(int maxport)
{
   int n;
   unsigned divisor;
   divisor = 115200L / 57600L;
   for (n=0;n<maxport;n++){
      OUTPORTB(com[n].base+3,0x80);
      OUTPORTB(com[n].base,divisor & 0x00ff);
      OUTPORTB(com[n].base+1,divisor >> 8);
      OUTPORTB(com[n].base+3,0x03); // 8n1+non-dlab
      OUTPORTB(com[n].base+4,0x09);             //out2+dtr
      OUTPORTB(com[n].base+1,0x01); // enable irqs
      INPORTB(com[n].base);// clear data and IRQ
      INPORTB(com[n].base);// clear data and IRQ
   }
}

void initvars(unsigned irqnum,int maxport)
{ FILE *cfg;
  char msg[10];
  int i=0;
  int j=0;
  IRQ2=irqnum;
  OUT=0;
  for(i=0;i<maxport;i++){
    com[i].base=Base[i];
    com[i].head=0;
    com[i].tail=0;
    com[i].x1=5;
    com[i].x2=80;
    com[i].y1=i*(24/maxport)+2;
    com[i].y2=com[i].y1+((24/maxport)-1);
    com[i].x=1;
    com[i].y=1;
    com[i].bc=(i&1)?CYAN:BLUE;
    com[i].fc=YELLOW;
    for(j=0;j<256;j++)
      com[i].data[j]=0;
  }
}
//automated shareirq test
int _16450ShareIRQ(unsigned Base1,unsigned Base2,unsigned Base3,unsigned Base4,unsigned Base5,
  unsigned Base6,unsigned Base7,unsigned Base8,unsigned irqnum,int maxport,int index)
{
  int test = 0;
  // test = _16450SharePCITest(Base1,Base2,Base3,Base4,Base5,Base6,Base7,Base8,irqnum,maxport,index);
  return test;
}


//debug shareirq test
#pragma argsused
int _16450ShareIRQDebug(unsigned Base1,unsigned Base2,unsigned Base3,unsigned Base4,unsigned Base5,
  unsigned Base6,unsigned Base7,unsigned Base8,unsigned irqnum,int maxport,int index,int df)
{
  int debug = 0;
  // debug = _16450SharePCI(Base1,Base2,Base3,Base4,Base5,Base6,Base7,Base8,irqnum,maxport);
  return debug;
}


int _16450SharePCI(unsigned Base1,unsigned Base2,unsigned Base3,unsigned Base4,unsigned Base5,
  unsigned Base6,unsigned Base7,unsigned Base8,unsigned irqnum,int maxport)
{
  unsigned done=0;
  Base[0]=Base1;
  Base[1]=Base2;
  Base[2]=Base3;
  Base[3]=Base4;
  Base[4]=Base5;
  Base[5]=Base6;
  Base[6]=Base7;
  Base[7]=Base8;
  numport = maxport;
  initvars(irqnum,maxport);
  drawscreen(maxport);
  // initirq(IRQ2,ISR);
  initcomms(maxport);
  DELAY(10);
  sendEOI();
  do{
     done+=handledata(maxport);
     done+=handlekeys(maxport);
  }while(!done);
  restoreirq(IRQ2);
  sendEOI();
  handledata(maxport);
  setFullWindow();
  CLRSCR();
  return 0;
}

//automated shareirq function
int _16450SharePCITest(unsigned Base1,unsigned Base2,unsigned Base3,unsigned Base4,unsigned Base5,
  unsigned Base6,unsigned Base7,unsigned Base8,unsigned irqnum,int maxport,int index)
{
  int done=0;
  int i = 0;
  Base[0]=Base1;
  Base[1]=Base2;
  Base[2]=Base3;
  Base[3]=Base4;
  Base[4]=Base5;
  Base[5]=Base6;
  Base[6]=Base7;
  Base[7]=Base8;
  numport = maxport; //set global variable numport to maxport
  initvars(irqnum,maxport);
  // initirq(IRQ2,ISR);
  initcomms(maxport);
  DELAY(10);
  sendEOI();
  done=handlekeys2(maxport,index);
  for(i = 0; i<maxport; i++)
    OUTPORTB(com[i].base+1,0x00); // disable irqs
  restoreirq(IRQ2);
  sendEOI();
  return done;
}






