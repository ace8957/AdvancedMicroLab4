/*
** Temp.c
** Converting the analog signal from a TC1047 Temperature Sensor
*/
#include <EX16.h>
#include <config.h>
#include <stdio.h>
#include "LCD.h"

#define TEMP_CH   4         // ch 4 = TC1047 Temperature sensor 
#define TEMPMASK  0xffef    // AN4 as analog input
#define NUM_SAMPLES   5
#define ERROR_BOUND 2
#define ERROR_DIFF 5

void displayInstructions(void) {
    putsLCD("Place finger on");
    SetLCDC(0x40);
    putsLCD("TC1047 to begin.");
}

int checkPrevious(int * a, int ref) {
    int i;
    for(i = 1; i < NUM_SAMPLES; ++i) {
        //return nonzero if the reading is outside of the ERROR_BOUND away from
        //the previous value in the buffer or if the value differs by less than
        //ERROR_DIFF from the reference value
        if((a[i] <= (a[i-1] + ERROR_BOUND)) || 
                (a[i] >= (a[i-1] - ERROR_BOUND)) ||
                (a[i] <= ref + ERROR_DIFF)  ||
                (a[i] >= ref - ERROR_DIFF)) {
            return 1;
        }
    }
    return 0;
}

void displayTemperature(int * a) {
    //display the temperature to the user
    //TODO: make this an actual celsius value
    //the temperature will be the average of those in the buffer
    int i;
    int temp = 0;
    char output[16];
    for(i = 0; i < NUM_SAMPLES; ++i) {
        temp += a[i];
    }
    temp /= NUM_SAMPLES;
    ClrLCD();
    SetLCDC(0x0);
    sprintf(output, "Temp: %d", temp);
    putsLCD(output);
}

int main ()
{
    int temp, ref, pos, j;
    int temps[NUM_SAMPLES];

    InitLCD();
    displayInstructions();
    
    // 1. initializations
    InitADC( TEMPMASK);  // initialize ADC Explorer16 inputs
    TRISA = 0xff00;      // select PORTA pins as outputs


    // 2. get the central bar reference
    temp = 0;
//    for ( j= 16; j >0; j--)
//         temp += ReadADC( TEMP_CH);  // read the temperature
//    ref = temp >> 4;
    ref = ReadADC(TEMP_CH);

    // 3.0 main loop
    j=0;
    while( 1)
    {

        temps[j] = ReadADC(TEMP_CH);
        j++;
        if(j == NUM_SAMPLES) j=0;

        if(!checkPrevious(temps, ref)) { //all are within bounds of each other
            displayTemperature(temps);
            Delayms(5000);
        }

        // 3.2 compare with initial reading, move bar 1 dot/C
        pos = 3 + (temp - ref);

        // 3.3 keep result in value range 0..7, keep bar visible
        if ( pos > 7)
            pos = 7;
        if ( pos < 0)
            pos = 0;

        // 3.4 turn on the corresponding LED
        PORTA = ( 0x80 >> pos);
    } // main loop
    return 0;
} // main
