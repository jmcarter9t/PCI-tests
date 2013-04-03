int _8255WalkingTest(unsigned base,int dac,int df);
int _8255IENTest(unsigned base, unsigned IRQ,int df,int d48,int cos, int hd);
int _8255COS(unsigned base,unsigned IRQ,int df);
int _8255Tst(unsigned base, int df);
int _8255TstPCIA1216A(unsigned base, int df);
int _8255notIENTest(unsigned base, unsigned IRQ, int df,int hd);
unsigned _8255PairPair(unsigned int Base,unsigned int IRQ,int df);
