void _ai(unsigned int base, unsigned int irq, unsigned int fifo, int df);
unsigned int aiexternal(unsigned int base, unsigned int IRQ,int df);
unsigned int ainotIRQ(unsigned int base, unsigned int IRQ,int df);
unsigned int aidigital(unsigned int base, unsigned int fifo,int df);
unsigned int aiout2(unsigned int base, unsigned int irq,int df);
unsigned aisoftware(unsigned BASE); //add debug stuff