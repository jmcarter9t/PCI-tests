void CtrMode(unsigned addr, char cntr, char mode);
void CtrLoad(unsigned addr ,int c,int val);
unsigned CtrRead(unsigned addr , int c);
int _8254Test(unsigned base,int df);
int _WDOGBitTest(unsigned base,unsigned irq,int df);
int _WDOGBuzzerTest(unsigned base,int df);
int WDOGCounterTest(unsigned base,int irq, int df);
unsigned int _8254CounterIEN(unsigned base, unsigned irq, unsigned F96,int df);