#include <dos.h>
#include <conio.h>
#include "irq0-15.h"

#define TRUE 1
#define FALSE 0

#define FILTER_FACTOR_MIN 30
#define FILTER_FACTOR_MAX 55

void IDO_Readback(int DebugFlag, unsigned short IDOAddr)
{
    int bFail = FALSE, bFailByte;

    textcolor(WHITE);
    cputs("IDO Readback Test\n\r");
    textcolor(LIGHTGRAY);
    for ( int ByteIndex = 0; ByteIndex <= 5; ++ByteIndex )
    {
        textcolor(LIGHTGRAY);
        bFailByte = FALSE;
        unsigned short Tar = IDOAddr + ByteIndex + ((ByteIndex >= 3) & 1);
        unsigned short DumAddr = IDOAddr + 3;
        unsigned char Data = 0;
        do
        {
            outportb(Tar, Data);
            outportb(DumAddr, ~Data);
            unsigned char InData = inportb(Tar);
            if ( InData != Data )
            {
                if ( DebugFlag ) cprintf("Port %c %s: wrote %02X, read %02X\n\r", 'A' + (ByteIndex >> 1), (ByteIndex & 1) ? "Hi" : "Lo", Data, InData);
                bFail = TRUE;
                bFailByte = TRUE;
            }
        } while ( ++Data != 0 );

        textcolor(bFailByte ? RED : GREEN);
        cprintf("      Port %c %s: %49s\n\r", 'A' + (ByteIndex >> 1), (ByteIndex & 1) ? "Hi" : "Lo", bFailByte ? "FAILED" : "PASSED");
    }

    textcolor(bFail ? RED : GREEN);
    cprintf("\n\r      Readback Test: %45s\n\r\n\r", bFail ? "FAILED" : "PASSED");
}

unsigned long IRQCount;
unsigned short IRQConfigAddr, IRQClearAddr;

void interrupt COSISR(...)
{
    if ( ++IRQCount == 0 ) --IRQCount;
    inportb(IRQClearAddr);
    sendEOI();
}

void AC_Inner(unsigned short oLo, unsigned short oHi, unsigned short iLo, unsigned short iHi, int TheTime[16])
{
    outportb(oLo, 0x00);
    outportb(oHi, 0x00);
    delay(5);

    for ( int Bit = 0; Bit <= 15; ++Bit )
    {
        unsigned short oM = 1 << Bit, iM;
        long int NumReads = 0;
        outportb(oLo, oM);
        outportb(oHi, oM >> 8);
        do
        {
            ++NumReads;
            iM = inportb(iLo) | (inportb(iHi) << 8);
        } while ( (iM != oM) && (NumReads < 30000) );

        TheTime[Bit] = ((iM == oM) ? NumReads : -1);
    }
}

#pragma argsused
void _IDO_Inner(int DebugFlag, char PortName, unsigned short oLo, unsigned short oHi, unsigned short iLo, unsigned short iHi, unsigned char IDIIRQ)
{
    int bFail = FALSE;
    int ShortTime[16], LongTime[16];
    long int aShort = 0, aLong = 0;

    textcolor(WHITE);
    cprintf("Port %c\n\r", PortName);
    textcolor(LIGHTGRAY);
    cprintf("Connect port %c of the IDO card and port A of the IDI card with the test jig, then press any key.", PortName);
    getch();
    cputs("\r                                                                              \r");

    for ( int Bit = 0; Bit <= 15; ++Bit )
    {
        unsigned short oM = 1 << Bit, iM;

        outportb(oLo, oM);
        outportb(oHi, oM >> 8);

        delay(5);

        iM = inportb(iLo) | (inportb(iHi) << 8);

        if ( iM != oM )
        {
            if ( DebugFlag ) cprintf("  wrote %04X, read %04X\n\r", oM, iM);
            bFail = TRUE;
        }
    }

    textcolor(bFail ? RED : GREEN);
    cprintf("\n\r      Port %c: %52s\n\r\n\r", PortName, bFail ? "FAILED" : "PASSED");
}

void IDI_IDO_Inner(int DebugFlag, char PortName, unsigned short oLo, unsigned short oHi, unsigned short iLo, unsigned short iHi, unsigned char IDIIRQ)
{
    int bFail = FALSE, bFailPass = FALSE;
    int ShortTime[16], LongTime[16];
    long int aShort = 0, aLong = 0;

    textcolor(WHITE);
    cprintf("Port %c\n\r", PortName);
    textcolor(LIGHTGRAY);
    cprintf("Connect port %c of the IDI and IDO cards with the test jig, then press any key.", PortName);
    getch();
    cputs("\r                                                                              \r");

    for ( int Bit = 0; Bit <= 15; ++Bit )
    {
        unsigned short oM = 1 << Bit, iM;

        outportb(oLo, oM);
        outportb(oHi, oM >> 8);

        delay(5);

        iM = inportb(iLo) | (inportb(iHi) << 8);

        if ( iM != oM )
        {
            if ( DebugFlag ) cprintf("  wrote %04X, read %04X\n\r", oM, iM);
            bFail = TRUE;
            bFailPass = TRUE;
        }
    }

    textcolor(bFailPass ? RED : GREEN);
    cprintf("      I/O Test: %50s\n\r", bFailPass ? "FAILED" : "PASSED");


    bFailPass = FALSE;
    outportb(oLo, 0x00);
    outportb(oHi, 0x00);
    delay(5);
    unsigned char OldMask = initirq(IDIIRQ, COSISR);
    outportb(IRQConfigAddr, 0xFF);
    delay(5);
    IRQCount = 0;
    textcolor(LIGHTGRAY);
    for ( Bit = 0; Bit <= 15; ++Bit )
    {
        unsigned short M = 1 << Bit;
        IRQCount = 0;
        delay(5);
        outportb(oLo, M);
        outportb(oHi, M >> 8);
        delay(5);
        outportb(oLo, 0x00);
        outportb(oHi, 0x00);
        delay(5);
        if ( IRQCount != 2 )
        {
            if ( DebugFlag ) cprintf("Port %c Bit %d: 2 edges, %ld IRQs\n\r", PortName, Bit, IRQCount);
            bFail = TRUE;
            bFailPass = TRUE;
        }
    }

    outportb(IRQConfigAddr, 0x00);
    delay(5);
    restoreirq(IDIIRQ, OldMask);
    for ( int I = 0; I <= 7; ++I )
        inportb(IRQConfigAddr - I)
    ;

    textcolor(bFailPass ? RED : GREEN);
    cprintf("      COS Test: %50s\n\r", bFailPass ? "FAILED" : "PASSED");



    textcolor(bFail ? RED : GREEN);
    cprintf("\n\r      Port %c: %52s\n\r\n\r", PortName, bFail ? "FAILED" : "PASSED");


    textcolor(LIGHTGRAY);
    cprintf("Make sure the filter jumpers for port %c are out, then press any key.", PortName);
    getch();
    cputs("\r                                                                              \r");

    AC_Inner(oLo, oHi, iLo, iHi, ShortTime);

    cprintf("Install the filter jumpers for port %c, then press any key.", PortName);
    getch();
    cputs("\r                                                                              \r");

    AC_Inner(oLo, oHi, iLo, iHi, LongTime);

    if ( DebugFlag ) cputs("       Bit   No Filt.   Filtered   Ratio\n\r");
    //                      ##########----------##########----------
    for ( Bit = 0; Bit <= 15; ++Bit )
    {
        if ( DebugFlag )
        {
            cprintf("%10d", Bit);
            if ( ShortTime[Bit] == -1 )
                cputs("   Timeout");
            else
                cprintf("%10d", ShortTime[Bit])
            ;
            if ( LongTime[Bit] == -1 )
                cputs("   Timeout");
            else
                cprintf("%10d", LongTime[Bit])
            ;
            if ( (ShortTime[Bit] != -1) && (LongTime[Bit] != -1) )
                cprintf("%10.1f\n\r", double(LongTime[Bit])/ShortTime[Bit]);
            else
                cputs("\n\r")
            ;
        }
        if ( (ShortTime[Bit] == -1) || (LongTime[Bit] == -1) )
        {
            bFail = TRUE;
            bFailPass = TRUE;
        }
        else
        {
            double D = LongTime[Bit];
            D /= ShortTime[Bit];
            if ( (D < FILTER_FACTOR_MIN) || (D > FILTER_FACTOR_MAX) )
            {
                bFail = TRUE;
                bFailPass = TRUE;
            }
        }
    }

    textcolor(bFailPass ? RED : GREEN);
    cprintf("      AC Filter Test: %44s\n\r", bFailPass ? "FAILED" : "PASSED");

    if ( DebugFlag )
    {
        textcolor(LIGHTGRAY);
        cputs("Press any key to continue.");
        getch();
        cputs("\r                                                                              \r");
    }

}

void IDI_IDO(int DebugFlag, unsigned short IDIAddr, unsigned short IDOAddr, unsigned char IDIIRQ)
{
    //DebugFlag = -1;

    IDO_Readback(DebugFlag, IDOAddr);
    IRQClearAddr = IDIAddr + 7;
    IRQConfigAddr = IDIAddr + 7;
    for ( int Port = 0; Port <= 2; ++Port )
    {
        unsigned short iLo, oLo, iHi, oHi;
        iLo = (Port * 2) + ((Port == 2) ?1:0);
        iHi = iLo + 1 + ((Port == 1) ?1:0);
        oLo = IDOAddr + iLo;
        oHi = IDOAddr + iHi;
        iLo = IDIAddr + iLo;
        iHi = IDIAddr + iHi;
        IDI_IDO_Inner(DebugFlag, 'A' + Port, oLo, oHi, iLo, iHi, IDIIRQ);
    }
}

void IDI_(int DebugFlag, unsigned short IDIAddr, unsigned short IDOAddr, unsigned char IDIIRQ)
{
    //DebugFlag = -1;

    IDO_Readback(DebugFlag, IDOAddr);
    IRQClearAddr = IDIAddr + 7;
    IRQConfigAddr = IDIAddr + 7;
    for ( int Port = 0; Port <= 2; ++Port )
    {
        unsigned short iLo, oLo, iHi, oHi;
        iLo = (Port * 2) + ((Port == 2) ?1:0);
        iHi = iLo + 1 + ((Port == 1) ?1:0);
        oLo = IDOAddr + 0;
        oHi = IDOAddr + 1;
        iLo = IDIAddr + iLo;
        iHi = IDIAddr + iHi;
        IDI_IDO_Inner(DebugFlag, 'A' + Port, oLo, oHi, iLo, iHi, IDIIRQ);
    }
}

void _IDO(int DebugFlag, unsigned short IDIAddr, unsigned short IDOAddr, unsigned char IDIIRQ)
{
    //DebugFlag = -1;

    IDO_Readback(DebugFlag, IDOAddr);
    IRQClearAddr = IDIAddr + 7;
    IRQConfigAddr = IDIAddr + 7;
    for ( int Port = 0; Port <= 2; ++Port )
    {
        unsigned short iLo, oLo, iHi, oHi;
        iLo = (Port * 2) + ((Port == 2) ?1:0);
        iHi = iLo + 1 + ((Port == 1) ?1:0);
        oLo = IDOAddr + iLo;
        oHi = IDOAddr + iHi;
        iLo = IDIAddr + 0;
        iHi = IDIAddr + 1;
        _IDO_Inner(DebugFlag, 'A' + Port, oLo, oHi, iLo, iHi, IDIIRQ);
    }
}
