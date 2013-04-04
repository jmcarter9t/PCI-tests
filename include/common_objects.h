#ifndef COMMON_OBJECTS_H
#define COMMON_OBJECTS_H

#include "data_types.h"
#include "pciutil.h"
#include "error_codes.h"

typedef enum {
  PCIeDIO24DS,OTHER, COM1S, COM2S, COM, PericomCOM4S, PericomCOM8S,
  COM8S, DIO, DIO22, IIRO8, WDGCSM, WDG2S, DA, AI, IIRO16, IDIO16,
  PCIA1216A, IDI, IDO, eDIO 
} infotype;

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
} tcardlist ;

struct globals {
  int DebugFlag;
  unsigned int Flag96;
  int result;
  dword vendev;
  byte bn;                 //bus_number
  byte df;                 //device_and_function
  byte regnum;
  word indexvalue;
  byte h;                  //temp values for debug
  int foundcount;
  word basea;
  word baseb;
  word basec;
  word based;
  word basee;
  word irqnum;
  int list_size;
  tcardlist *list;
};

extern tcardlist list[];
extern struct globals GLOBALS;


/* tcardlist list[]; */
#endif
