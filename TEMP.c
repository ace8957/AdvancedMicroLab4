/*
** Temp.c
** Converting the analog signal from a TC1047 Temperature Sensor
*/
#include "EX16.h"
#include <config.h>
#include <stdio.h>
#include "LCD.h"

#define TEMP_CH   4         // ch 4 = TC1047 Temperature sensor 
#define TEMPMASK  0xffef    // AN4 as analog input
#define NUM_SAMPLES   5
#define ERROR_BOUND 4
#define ERROR_DIFF 3

void displayInstructions(void) {
    ClrLCD();
    SetLCDC(0x0);
    putsLCD("Place finger on");
    SetLCDC(0x40);
    putsLCD("TC1047 to begin.");
    Delayms(1000);
}

void displayErrorMessage(void) {
    ClrLCD();
    SetLCDC(0x0);
    putsLCD("Error");
    SetLCDC(0x40);
    putsLCD("Leave Finger in");
    Delayms(2000);
    SetLCDC(0x0);
    putsLCD("Position for");
    SetLCDC(0x40);
    putsLCD("a Longer Time.  ");
    Delayms(2000);
    ClrLCD();
}

int checkPrevious(int * a, int ref) {
    int i, j;
    
    for(i = 0; i < NUM_SAMPLES; ++i) {
        //check to make sure all values differ from the ref by at least
        //ERROR_DIFF amount
        if((a[i] > (ref - ERROR_DIFF)) && (a[i] < (ref + ERROR_DIFF))) {
            return 1;
        }
        for(j = 0; j < NUM_SAMPLES; ++j) {
            //check if not within error bound of other values
            //if we have several values which meet the criteria,
            //leave finger on the sensor longer
            if((a[i] < (a[j] - ERROR_BOUND)) || (a[i] > (a[j] + ERROR_BOUND))) {
                return 1;
            }
            
        }
    }
    
    return 0;
}

int withinTolerance(int t, int ref) {
    if((t > (ref - ERROR_DIFF)) && (t < (ref + ERROR_DIFF))) {
            return 1;
    }
    return 0;
}

void displayTemperature(int * a) {
    //display the temperature to the user
    int i;
    int temp = 0;
    char output[16];
    for(i = 0; i < NUM_SAMPLES; ++i) {
        temp += a[i];
    }
    temp /= NUM_SAMPLES;
    ClrLCD();
    SetLCDC(0x0);
    sprintf(output, "Temp: %.2f Cel.", convertRaw(temp));
    putsLCD(output);
}

int main ()
{
    int temp, ref, j;
    int display_flag = 0;//flag to regulate temp display
    int buffer_flag = 0;//flag to regulate filling of the buffer
    int temps[NUM_SAMPLES] = {0};//buffer of raw adc readings

    InitLCD();//initialize the LCD
    displayInstructions();//display the instructions
    
    InitADC( TEMPMASK);  // initialize ADC Explorer16 inputs

    Delayms(125);//wait for buffer to fill

    ref = ReadADC(TEMP_CH);//get an initial reference point

    //main loop
    j=0;//zero the buffer counter
    while(1)
    {

        temp = ReadADC(TEMP_CH);//get adc reading
        if(!withinTolerance(temp,ref) && !buffer_flag) {//check tolerances
            buffer_flag = 1;
            j = 0;
        }
        if(buffer_flag) {//temp outside normal tolerance, begin filling buffer
            if(withinTolerance(temp, ref)) {//no longer outside tolerance
                buffer_flag = 0;
                displayErrorMessage();
                displayInstructions();
            }
            else {
                temps[j] = temp;
                j++;
                if(j==NUM_SAMPLES) j=0;
            }
        }

        //all are within bounds of each other and outside min. tolerance
        if(!checkPrevious(temps, ref) && buffer_flag && j==(NUM_SAMPLES-1)) {
            buffer_flag = 0;
            displayTemperature(temps);//display the new temperature
            if(display_flag)
                Delayms(3000);
            else {
            Delayms(10000);//continue to display temp for readability
            display_flag = 1;
            }
            continue;
        }

        if(display_flag) {//display instructions again
            display_flag = 0;
            displayInstructions();
        }

        Delayms(1000);//wait to allow for even temperature

    } // main loop
    return 0;
} // main
