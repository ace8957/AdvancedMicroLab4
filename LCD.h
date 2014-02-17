#ifndef LCD_H
#define LCD_H

#define LCDDATA 1
#define LCDCMD  0
#define PMDATA  PMDIN1

void InitLCD( void);
char ReadLCD( int);
void WriteLCD( int, char);

#define putLCD( d)  WriteLCD( LCDDATA, (d))
#define CmdLCD( c)  WriteLCD( LCDCMD, (c))
#define HomeLCD()   WriteLCD( LCDCMD, 2)
#define ClrLCD()    WriteLCD( LCDCMD, 1)
#define SetLCDC( a) WriteLCD( LCDCMD, (a & 0x7F) | 0x80)
#define SetLCDG( a) WriteLCD( LCDCMD, (a & 0x3F) | 0x40)

void putsLCD( char *);

#define TFLY 9000       // 9000 x 16us = 144ms
#define TLONG 32000
#define DELAY() TMR1=0; while( TMR1<TFLY)
#define LDELAY() TMR1=0; while( TMR1<TLONG); TMR1=0; while( TMR1<TLONG)

void loadCustomCharacter(char*);

#endif