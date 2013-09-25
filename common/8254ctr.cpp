#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include "display.h"
#include "io.h"
#include "data_types.h"
#include "common_objects.h"
#include "irq.h"

//Counter Driver for the 8254 chip
//#include <dos.h>
//Base Address passed is actually base the offset for the address of the
//counter

unsigned read_status(unsigned BaseAddress)
{
   unsigned status;

   OUTPORTB(BaseAddress + 3, 0xE4);           // 1110 0100 ReadBack for ctr1
   status = INPORTB(BaseAddress + 1);         // ctr1 output
   status = status >> 7;

   return(status);
}

unsigned get_count(unsigned BaseAddress)
{
   unsigned counted=0, LSB=0, MSB=0, x;
   OUTPORTB(BaseAddress + 3, 0xD2);           // 1101 0010
   LSB = INPORTB(BaseAddress);                // LSB of ctr0
   MSB = INPORTB(BaseAddress);                // MSB of ctr0

   counted = (MSB << 8) + LSB;            // values at ctr1 = 0
   return(counted);
}

long frequency_measure(unsigned BaseAddress)
{
   unsigned secondcount=0;
   double frequency;
   long returnval;
   unsigned long timeout = 655350L;
   OUTPORTB(BaseAddress + 3, 0x76);           // 0111 0110 ctr1 mode3, LSB/MSB
   OUTPORTB(BaseAddress + 1, 0xFE);           // load LSB and MSB values into
   OUTPORTB(BaseAddress + 1, 0xFF);           // ctr1 to make slow input to gate 0

   while (read_status(BaseAddress) && timeout--);      // out ctr1 = 1
   timeout = 655350L;
   OUTPORTB(BaseAddress + 3, 0x34);           // 0011 0100 ctr0 mode 2 L/M
   OUTPORTB(BaseAddress, 0xFF);
   OUTPORTB(BaseAddress, 0xFF);               // load 65535 into ctr 0
   while (!(read_status(BaseAddress)) && timeout--);   // ctr1 still 0
   timeout = 655350L;
   while (read_status(BaseAddress) && timeout--);      // ctr1 = 1
   timeout = 655350L;
   secondcount = get_count(BaseAddress);  // read value of counter
   frequency = (65535 - secondcount)/0.032767;   // convert from counts to f
   returnval = frequency;                 // convert from double to int
   return(returnval);
}

void init_counter(unsigned BaseAddress)
{
   OUTPORTB(BaseAddress + 3, 0x30);           // 0011 0000 ctr0 LSB/MSB mode 0
   OUTPORTB(BaseAddress, 0xFF);
   OUTPORTB(BaseAddress, 0xFF);               // load 65535 into ctr0
}

unsigned read_counter(unsigned BaseAddress)
{
   int countval;

   OUTPORTB(BaseAddress + 3, 0xD2);           // 1101 0010 RB ctr0 latch
   countval = INPORTB(BaseAddress);           // LSB
   countval += (INPORTB(BaseAddress) << 8);   // MSB

   return (countval);
}

unsigned event_counter(unsigned BaseAddress, int feature)
{
   const   int INITIALIZE = 1;                // initialize the counter
   const   int START      = 2;                // start counting
   const   int SINCESTART = 4;                // how many since starting
   const   int SINCELAST  = 8;                // how many since last check
   const   int STOP       = 16;               // stop counting
   const   int RESET      = 32;               // reset counter to 65535

   static unsigned previousmeasure = 0;
   unsigned currentmeasure, returnval;
   int flag = 0;

   if (INITIALIZE & feature)              // 1
   {
      init_counter(BaseAddress);          // init counter0 to count in mode 0
   }

   if (START & feature)                   // 2
   {
      OUTPORTB(BaseAddress + 3, 0x74);        // 0111 0010 ctr1 LSB/MSB mode 1
					  // to hold gate of ctr0 high
   }

   if (SINCESTART & feature)              // 4
   {
      if (!flag)                          // counts down from 65535
      {
	 currentmeasure = 65535 - read_counter(BaseAddress);
	 flag = 1;                        // flag read counter
      }
      returnval = currentmeasure;         // set value to return
   }

   if (SINCELAST & feature)               // 8
   {
      if (!flag)
      {                                   // calculate counts
	 currentmeasure = 65535 - read_counter(BaseAddress);
	 flag = 1;
      }                                   // calculate since last
      returnval = currentmeasure - previousmeasure;
   }

   if (STOP & feature)                    // 16
   {
      OUTPORTB(BaseAddress + 3, 0x70);        // 0111 0000 ctr1 mode0 out=0
   }

   if (RESET & feature)                   // 32
   {
      init_counter(BaseAddress);          // init counter to 65535
      previousmeasure = 0;                // no previous counts
   }

   if (flag)                              // set previous measure if counter read
      previousmeasure = currentmeasure;
   if (flag)
      return(returnval);                  // if counter read return value
   else
      return (0);                         // return 0 if counter not read
}

long round(float number)                  // round off the floats
{
   float compareval;
   long returnval;

   compareval = (long)(number);             // convert to long
   if (number > (compareval + 0.5))       // round up for > 0.5
      returnval = compareval + 1;
   else
      returnval = compareval;             // round down for <= 0.5
   return (returnval);                    // return long
}

void generatefrequency(unsigned BaseAddress, unsigned long frequency)
{
   unsigned int countsA, countsB;
   long temp;
   float trash = 0;
   unsigned int x = 2;
					  // return if freq is too low/high
   if ((frequency > 250000) || (frequency < 1))
      return;

   trash = (float)(frequency);              // convert to float for division
   trash = 1000000/trash;                 // calculate the number of counts
   temp = round(trash);                   // round to the nearest count
   do
   {                                      // try to divide the counts
      countsA = x;                        // evenly between the two counters
      x++;
   }                                      // exit if counts too high
   while ((temp % countsA) && (countsA < temp) && (countsA < 65535));

   if ((countsA >= temp) && (temp < 131070))
   {                                      // went through all vals
      countsA = 2;                        // counter can only hold 65535
      countsB = temp/2;
   }
   else
   {
      if (temp >= 131070)                 // counts too high to fit in counter
      {
	 countsA = 20;                    // divide to make fit
	 countsB = temp / 20;
      }
      else
	 countsB = temp / countsA;        // if found divisor use it
   }
   OUTPORTB(BaseAddress + 3, 0xBE);           // 1011 1110 ctr2 mode3
   temp = countsA & 0x00FF;
   OUTPORTB(BaseAddress + 2, temp);           // write the low byte
   temp = countsA >> 8;
   temp &= 0x00FF;
   OUTPORTB(BaseAddress + 2, temp);           // write the high byte

   OUTPORTB(BaseAddress + 3, 0x74);           // 0111 0100 ctr1 mode2
   temp = countsB & 0x00FF;
   OUTPORTB(BaseAddress + 1, temp);           // write the low byte
   temp = countsB >> 8;
   temp &= 0x00FF;
   OUTPORTB(BaseAddress + 1, temp);           // write the high byte

}

unsigned total_count(unsigned BaseAddress)
{
   unsigned LSB, MSB;
   OUTPORTB(BaseAddress + 3, 0xD4);           // 1101 0100 Read Back counts ctr1
   LSB = INPORTB(BaseAddress + 1);            // ctr1 counts LSB
   MSB = INPORTB(BaseAddress + 1);            // ctr1 counts MSB

   return (MSB << 8) + LSB;
}

unsigned count2(unsigned BaseAddress)
{
   unsigned LSB, MSB;
   OUTPORTB(BaseAddress + 3, 0xD8);           // 1101 1000 Read Back counts ctr1
   LSB = INPORTB(BaseAddress + 2);            // ctr1 counts LSB
   MSB = INPORTB(BaseAddress + 2);            // ctr1 counts MSB

   return (MSB << 8) + LSB;
}

double pulse_measure(unsigned BaseAddress)
{
   #define TIMEOUTVAL  150000
   unsigned firstcount = 0;
   unsigned secondcount = 0;
   unsigned previouscount = 0;
   unsigned time = 0;
   unsigned long timeout;

   unsigned one, two;

   OUTPORTB(BaseAddress + 3, 0xB4);           // 1011 0100 ctr2 mode2
   OUTPORTB(BaseAddress + 2, 0xFF);           // write 65535 to counters
   OUTPORTB(BaseAddress + 2, 0xFF);

   OUTPORTB(BaseAddress + 3, 0x72);           // 0111 0010 ctr1 mode2
   DELAY(1);
   OUTPORTB(BaseAddress + 3, 0x70);           // 0111 0000 ctr1 mode2
   DELAY(1);
   OUTPORTB(BaseAddress + 3, 0x74);           // 0111 0100 ctr1 mode2
   OUTPORTB(BaseAddress + 1, 0xFF);           // write 65535 to counters
   OUTPORTB(BaseAddress + 1, 0xFF);

   firstcount = total_count(BaseAddress); // read the counter
   timeout = 0;
   do
   {
      previouscount = firstcount;
      firstcount = total_count(BaseAddress);
      timeout++;
   }
   while ((firstcount != previouscount) && (timeout < TIMEOUTVAL));
					  // check to see if the counts are changing
   one = count2(BaseAddress);
   if (timeout < TIMEOUTVAL)
      timeout = 0;
   do
   {
      secondcount = total_count(BaseAddress);
      timeout++;
   }
   while ((firstcount == secondcount) && (timeout < TIMEOUTVAL));
					  // check to see if counts are stable
   if (timeout < TIMEOUTVAL)
      timeout = 0;
   do
   {
      previouscount = secondcount;
      DELAY(1);
      secondcount = total_count(BaseAddress);
      timeout++;
   }
   while ((secondcount != previouscount) && (timeout < TIMEOUTVAL));
					  // check to see if counts are changing
   two = count2(BaseAddress);
   if (timeout < TIMEOUTVAL)
   {
      time = firstcount - secondcount;
      return(time);                       // return the counts for the pulse
   }
   else
      return 0;
}

void syncronize(unsigned BaseAddress)
{
   unsigned secondcount = 0;
   unsigned previouscount = 0;
   unsigned timeout = 500;

   secondcount = total_count(BaseAddress);
   do
   {
      previouscount = secondcount;
      DELAY(1);
      secondcount = total_count(BaseAddress);
   }
   while ((secondcount != previouscount) && timeout--);
}


double pulse_width(unsigned BaseAddress)
{
  unsigned temp = 0;
  syncronize(BaseAddress);
  temp = pulse_measure(BaseAddress);
  return(temp);
}
