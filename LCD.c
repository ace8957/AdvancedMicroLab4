/*
** Glass.c
** Playing with an alphanumeric LCD display
*/

//#include <config.h>
#include <p24Fxxxx.h>
#include "LCD.h"

//#define LCDDATA 1
//#define LCDCMD  0
//#define PMDATA  PMDIN1

void InitLCD( void)
{
    // PMP initialization 
    PMCON = 0x83BF;             // Enable the PMP, long waits
    PMMODE = 0x3FF;             // Master Mode 1
    PMAEN = 0x0001;             // PMA0 enabled
    
    // init TMR1
    T1CON = 0x8030;             // Fosc/2, 1:256, 16us/tick

    // wait for >30ms
    TMR1 = 0; while( TMR1<2000);// 2000 x 16us = 32ms   
    
    //initiate the HD44780 display 8-bit init sequence
    PMADDR = LCDCMD;            // command register
    PMDATA = 0b00111000;        // 8-bit, 2 lines, 5x7
    TMR1 = 0; while( TMR1<3);   // 3 x 16us = 48us   
    
    PMDATA = 0b00001100;        // ON, cursor off, blink off
    TMR1 = 0; while( TMR1<3);   // 3 x 16us = 48us   
    
    PMDATA = 0b00000001;        // clear display
    TMR1 = 0; while( TMR1<100); // 100 x 16us = 1.6ms   
    
    PMDATA = 0b00000110;        // increment cursor, no shift
    TMR1 = 0; while( TMR1<100); // 100 x 16us = 1.6ms   
} // InitLCD


char ReadLCD( int addr)
{
    int dummy;
    while( PMMODEbits.BUSY);    // wait for PMP to be available
    PMADDR = addr;              // select the command address
    dummy = PMDATA;             // initiate a read cycle, dummy
    while( PMMODEbits.BUSY);    // wait for PMP to be available
    return( PMDATA);            // read the status register
    
} // ReadLCD

#define BusyLCD() ReadLCD( LCDCMD) & 0x80
#define AddrLCD() ReadLCD( LCDCMD) & 0x7F
#define getLCD()  ReadLCD( LCDDATA)

void WriteLCD( int addr, char c)
{
    while( BusyLCD());
    while( PMMODEbits.BUSY);    // wait for PMP to be available
    PMADDR = addr;
    PMDATA = c;
} // WriteLCD
    

//#define putLCD( d)  WriteLCD( LCDDATA, (d))
//#define CmdLCD( c)  WriteLCD( LCDCMD, (c))
//#define HomeLCD()   WriteLCD( LCDCMD, 2)
//#define ClrLCD()    WriteLCD( LCDCMD, 1)
//#define SetLCDC( a) WriteLCD( LCDCMD, (a & 0x7F) | 0x80)
//#define SetLCDG( a) WriteLCD( LCDCMD, (a & 0x3F) | 0x40)

void putsLCD( char *s)
{
    while( *s) putLCD( *s++);
} //putsLCD


//#define TFLY 9000       // 9000 x 16us = 144ms
//#define TLONG 32000
//#define DELAY() TMR1=0; while( TMR1<TFLY)
//#define LDELAY() TMR1=0; while( TMR1<TLONG); TMR1=0; while( TMR1<TLONG)

//place a custom character from an 8-byte buffer
void loadCustomCharacter(char* c) {
    int i;
    for(i = 0; i < 8; ++i) {
        putLCD(c[i]);
    }
}
