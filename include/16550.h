void DTRTSTest(unsigned short *base, int index, unsigned char OutByte, unsigned char InNybble, const char* TestName);

int _16450DualTest(unsigned base, unsigned base2,int df);
int _16450FullDuplex(unsigned base,int df);
int _16450WDGFullDuplex(unsigned base,int df);
int _16450COM1SFullDuplex(unsigned base,int df);
int _16450IRQTest(unsigned base, unsigned irq, int df);
int _16450COM1SIRQTest(unsigned base, unsigned irq, int df);
int _16450NotIRQTest(unsigned base, unsigned irq, int df);
int _16450HalfDuplex(unsigned Base1, unsigned Base2,int df);
#define RECEIVE     0           /* Receiver buffer register              */
#define TRANSMIT    0           /* Transmitter holding register          */
#define L_DIVISOR   0           /* Divisor latch, least significant byte */
#define U_DIVISOR   1           /* Divisor latch, most significant byte  */
#define INT_ENB     1           /* Interrupt enable register             */
#define INT_ID      2           /* Interupt identification register      */
#define LINE_CTRL   3           /* Line control register                 */
#define MODEM_CTRL  4           /* Modem control register                */
#define LINE_STAT   5           /* Line status register                  */
#define MODEM_STAT  6           /* Modem status register                 */
