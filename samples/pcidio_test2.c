 /****************************************************************************
 *      sf.c for PCI-DIO-24                                                  *
 *      must be in su or root mode                                           *
 *                                                                           *
 *                          MODULE: SAMPLE0.C                                *
 *                                                                           *
 *  This sample program will sequentially turn on all bits in port a and then*
 *  seqencially turn them off.  Each time it sets a new bit, both port a and *
 *  port b are read and the data displayed.  This demonstrates how to read   *
 *  and write to a port, and to use the read back function of the 8255 chip. *
 *  If the port a pins are jumpered to the port b pins, then a board test    *
 *  program results, with port b being used to verify what has been written  *
 *  to port a.  The program will use port 0 of cards with mulitple 8255's.   *
 *                                                                           *
 *  LAST MODIFICATION: 8/30/01                                               *
 *                                                                           *
 *****************************************************************************/


#include <stdio.h>
#include <stdlib.h>
#include <sys/io.h>
#include <ctype.h>

/* these are the port address offsets for reading and control of the IOD */
unsigned int PortA;
unsigned int PortB;
unsigned int PortC;
unsigned int PortControl;

/* this typedef allow use to duplicate the boolean type as used in pascal */

typedef enum {FALSE,TRUE} boolean;

/*****************************************************************************
 *  FUNCTION: signon -- local routine                                        *
 *                                                                           *
 *   PURPOSE: To display an initial sign on message on the screen.           *
 *                                                                           *
 *     INPUT: None.                                                          *
 *                                                                           *
 *    OUTPUT: None.                                                          *
 *                                                                           *
 *****************************************************************************/
void signon(void)
{
#define L1 " D I O  C A R D  S A M P L E  P R O G R A M "
#define L2 " press any key and ENTER to exit sample program"
#define L3 " Connect a loopback cable from PORT A to PORT B of PPI0."
#define L4 " PORT A OUTPUT     PORT A INPUT      PORT B INPUT "
#define L5 " -------------     ------------      ------------ "
#define L6 " just press ENTER to print another output or "
#define L7 " then press any key to continue"

   printf("%s\n",L1);
   printf("%s\n",L3);
   printf("%s\n\n",L7);
   getchar();
   printf("%s\n",L2);
   printf("%s\n\n",L6);
   printf("%s\n",L4);
   printf("%s\n",L5);
} /* end signon */


/*****************************************************************************
 *  FUNCTION: write_port_data -- local routine                               *
 *                                                                           *
 *   PURPOSE: Displays the current port values on the screen.                *
 *                                                                           *
 *     INPUT: current: array with the port reading to display.               *
 *                                                                           *
 *****************************************************************************/
void write_port_data(unsigned char current[3])
{
unsigned index1,index;
unsigned char value;

    putchar(' ');
    putchar(' ');
    putchar(' ');

    for (index = 0;index <= 2;index++)       /* for each array member */
    {
        value = current[index];
        for (index1 = 0;index1 <= 7;index1++) /* for each bit in array member */
        {
            if (value % 2) putchar('1');
            else putchar('0');
            value = value >> 1;                /* roll next diaplay bit */
        }
        putchar(' ');
        putchar(' ');
        putchar(' ');
        putchar(' ');
        putchar(' ');
        putchar(' ');
        putchar(' ');
        putchar(' ');
        putchar(' ');
        putchar(' ');
    }
    printf("\n");
}  /* end write_port_data */


unsigned int AskForBaseAddress(unsigned int OldOne){
   char msg[4];
   unsigned int NewOne = 0;
   int index = 0;
   int letter;
   puts("Please enter Base Address of Data Card");
   puts("Input should be 4 HEX numbers: 03ac, f82d, etc");
   puts("Then press ENTER");
   while((letter = getchar())!='\n')
        msg[index++] = letter;
   sscanf(msg, "%x", &NewOne);
   return((NewOne>=0x1000)&&(NewOne<=0xf3f0)) ? NewOne : OldOne;
}//Ask

void IOPermission()
{
    if (iopl(3) < 0 )
    {
        fprintf(stderr, "NO IO PERMISSION\n");
        exit(EXIT_FAILURE);
    }else
    puts("Write permission granted to I/O Ports");
}//IOPermission

int main(void)
{
   unsigned char current[3];
   boolean shift_left;
   int address = 0xe400;
   puts("\n\n\n\n");
   puts("                      SAMPLE 1.C : DIO READ/WRITE                    ");
   puts("");
   printf("This sample program will sequentially turn on bits in port a and then ");
   puts("sequentially turn them off.  Each time a new bit sets, port a and ");
   puts("port b are read and data displayed.  This demonstrates how to read ");
   puts("and write to ports, and use the read back function of the 8255 chip.\n");


   PortA = AskForBaseAddress( address );
   PortB = PortA + 1;
   PortC = PortA + 2;
   PortControl = PortA + 3;
   printf("Address entered was %x hexadecimal,\n", PortA);
   puts("if this is correct press ENTER");
   puts("if this is incorrect, press N then ENTER");
   if(toupper(getchar())=='N')
   {
        puts("\n\nPlease restart Program and enter correct address.\n\n");
        exit(1);
   }//if
   IOPermission();
   signon();
   outb(0x8b,PortControl);
   current[0] = 0;
   shift_left = TRUE;
   while(getchar() == '\n')
   {
      outb(current[0],PortA);
      current[1] = inb(PortA);
      current[2] = inb(PortB);
      write_port_data(current);
      if(current[0] == 0)
         shift_left = TRUE;
      if(current[0] == 255)
         shift_left = FALSE;
      if(shift_left)
         current[0] = (current[0]<<1) + 1;
      else
         current[0] = (current[0] - 1)>>1;
   };//while
   printf("\n\n    end sample program\n");
   iopl(0);
   return 0;
}//main
