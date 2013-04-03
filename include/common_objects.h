#ifndef _COMMON_OBJECTS_H
#define _COMMON_OBJECTS_H

#include "data_types.h"
#include "pcilib.h"
#include "error_codes.h"

int DebugFlag;
unsigned int Flag96 = 0;
int result = 0;
dword vendev=0L;
byte bn=0;                           //bus_number
byte df = 0;                         //device_and_function
byte regnum;
word indexvalue;
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


#endif
