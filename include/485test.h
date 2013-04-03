#ifndef _485_TEST_H
#define _485_TEST_H
int _16450ShareIRQ(unsigned baseA,unsigned baseB,unsigned baseC,unsigned baseD,
  unsigned baseE,unsigned baseF,unsigned baseG,unsigned baseH,unsigned irq,
  int numport,int index);
int _16450ShareIRQDebug(unsigned baseA,unsigned baseB,unsigned baseC,unsigned baseD,
  unsigned baseE,unsigned baseF,unsigned baseG,unsigned baseH,unsigned irq,
  int numport,int index,int df);
int _16450SharePCITest(unsigned baseA,unsigned baseB,unsigned baseC,unsigned baseD,
  unsigned baseE,unsigned baseF,unsigned baseG,unsigned baseH,unsigned irq,
  int numport,int index);
  int _16450SharePCI(unsigned baseA,unsigned baseB,unsigned baseC,unsigned baseD,
  unsigned baseE,unsigned baseF,unsigned baseG,unsigned baseH,unsigned irq,
  int numport);

#endif
