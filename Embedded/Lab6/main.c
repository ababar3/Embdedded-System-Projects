/*
 * main.c for ECE 266 Lab 3, stopwatch
 * Arsalan Babar and Imran Babar
 *  Created on: Aug 23, 2016
 *  Last updated on: Sep 19, 2017
 *      Author: zzhang
 */
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include "launchpad.h"
#include "seg7.h"
#include "seg7digit.h"
#include "Rotary.h"
#include <driverlib/adc.c>
#include <driverlib/adc.h>
#include <math.h>


int value; // gave a variable for ADCRead()

// The running state of the stopwatch system
enum {Raw, Percent}  sysState = Percent;   // When one resets the controller, it would automatically go into percent mode.

// The initial state of the 7-segment display: "00:00" with colon on
 seg7Display_t seg7Display = {1, 0, 0, 0, 0};  //colon has been turned off.

void
displayMode(uint32_t time)
{
    if (sysState == Raw){

        value = ADCRead();  // this would be done directly.

        seg7Display.d1 = value % 10;

        if (value < 1000)
                    seg7Display.d4 = 10;
                else{
                    seg7Display.d4 = (value/1000) %10;
                }
        if (value< 100)
            seg7Display.d3 =10;
        else {
            seg7Display.d3 = (value/100) %10;
        }
        if(value < 10)  //
                 seg7Display.d2 =10;
          else{
              seg7Display.d2 = (value/10) %10;
          }

            //This properly adjusts the values for the display in raw mode. Turns on the right digits for the display.
        }

else  if (sysState == Percent)
    {
        value = ADCRead();
        value = value*100/4095;  // calculates the percentage out of the total 4095.

        seg7Display.d4 = 10;
        seg7Display.d1 = value % 10;

        if(value < 10)
               seg7Display.d2 =10;
        else{
            seg7Display.d2 = (value/10) %10;
        }
        if (value< 100)
            seg7Display.d3 =10;
        else {
            seg7Display.d3 = (value/100) %10;
        }

}


    seg7DigitUpdate(&seg7Display);
    schdCallback(displayMode, time + 200);
}

// Callback function for checking push button. SW1 is the RESET button, and
// SW2 is the START/PAUSE/RESUME button.
void
checkPushButton(uint32_t time)
{
    int code = pbRead();
    uint32_t delay;

    switch (code) {
    case 1:                         // SW1 is the checkpush button to get into the percent mode
        sysState = Percent;
        delay = 250;                // software debouncing
        break;

    case 2:                         // SW2 is the checkpush button to get into the Raw mode

        sysState = Raw;
        delay = 250;                // software debouncing
        break;

    default:
        delay = 10;
    }

    schdCallback(checkPushButton, time + delay);
}

int main(void)
{
    lpInit();
    seg7Init();
    ADCInit();
    uprintf("%d\n\r",ADCRead()); //prints out the initial value of the analog
    uprintf("%s\n\r", "Lab 6: ADC");


    // Schedule the first callback events for LED flashing and push button checking.
    // Those trigger callback chains. The time unit is millisecond.
    schdCallback(checkPushButton, 1000);
    schdCallback(displayMode, 1000);

    // Run the event scheduler to process callback events
    while (true) {
        schdExecute();
    }
}
