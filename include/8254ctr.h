#ifndef _8254CTR_H
#define _8254CTR_R
// 8254 Counter Driver
// Header file

long frequency_measure(unsigned BaseAddress);
//********Determines an unknow input frequency*********
//input : accepts the Base Address of the counter
//        clk pin accepts the unknown
//        frequency (must be less than 1MHz)
//
//output : returns the frequency (in Hz)

unsigned event_counter(unsigned BaseAddress, int feature);
//********Counts the number of events******************
//input : accepts the Base Address of the counter
//        feature is a bitwise OR of the following
//        values (this allows multiple commands with a
//        single call, but the function will only return a
//        single value
//    INITIALIZE = 1;  initialize the counter
//    START      = 2;  begin counting
//    SINCESTART = 4;  return the number of events since the start
//    SINCELAST  = 8;  return the number of events since last check
//    STOP       = 16; stop counting events
//    RESET      = 32; reset number of events to 0
//
//        clk pin accepts the incoming events
//
//output : returns the number of events

void generatefrequency(unsigned BaseAddress, unsigned long frequency);
//********Generates a Square Wave frequency**************
//input : accepts as input the Base Address of counter 0
//        and the desired frequency in Hz
//
//output : the frequency is generated on the out pin.

unsigned pulse_width(unsigned BaseAddress);
//********Pulse Width measurement************************
//input : accepts as input the Base Address of counter 0
//        and a slow changing signal on the gate pin
//        of counter 1.
//
//output : the width in seconds of the pulse is returned

#endif
