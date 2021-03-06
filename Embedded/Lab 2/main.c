/*
 * main.c
 *
 *  Created on: Aug 23, 2016
 *      Author: zzhang
 *      Arsalan Babar and Imran Babar
 */

#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <driverlib/sysctl.h>
#include <inc/hw_ints.h>
#include <inc/hw_memmap.h>
#include <inc/hw_i2c.h>
#include <driverlib/gpio.h>
#include <driverlib/pin_map.h>
#include <driverlib/i2c.h>
#include "launchpad.h"
#include "seg7.h"

// 7-segment coding table. See https://en.wikipedia.org/wiki/Seven-segment_display. The segments
// are named as A, B, C, D, E, F, G. In this coding table, segments A-G are mapped to bits 0-7.
// Bit 7 is not used in the coding. This display uses active high signal, in which '1' turns ON a
// segment, and '0' turns OFF a segment.
static uint8_t seg7Coding[10] = {
        0b00111111,         //digit 0
        0b00000110,         //digit 1
        0b01011011,         //digit 2
        0b01001111,         //digit 3
        0b01100110,         //digit 4
        0b01101101,         //digit 5
        0b01111101,         //digit 6
        0b00000111,         //digit 7
        0b01111111,         //digit 8
        0b01101111,         //digit 9
        // MORE CODINGS
};

// The colon status: if colon == 0b10000000, then the colon is on,
// otherwise it is off.
static uint8_t colon = 0;

//Counter is made for seconds
uint16_t secondOne = 0;
uint16_t secondTwo = 0;


//Counter is made for minutes
uint16_t minuteOne = 0;
uint16_t minuteTwo=0;
//colon counter
uint16_t colCounter=0;

// Update the clock display
void
clockUpdate(uint32_t time)  // pointer to a 4-byte array 4-byte array
{
    uint8_t code[4]; // The 7-segment code for the four clock digits

    // Display 01:23 on the 7-segment displays
    // The colon ':' will flash on and off every 0.5 seconds

    if(secondOne>=10){
        secondTwo++;
        secondOne=0;
    }
    if(secondTwo>=6){
        secondTwo=0;
        minuteOne++;
    }
    if(minuteOne>=10){
        minuteOne=0;
        minuteTwo++;
    }
    if(minuteTwo>=6){
       minuteTwo=0;
    }
        code[0] = seg7Coding[secondOne] + colon;
        code[1] = seg7Coding[secondTwo] + colon;
        code[2] = seg7Coding[minuteOne] + colon;
        code[3] = seg7Coding[minuteTwo] + colon;
        seg7Update(code);
    // Calculate the display digits and colon setting for the next update
    if (colon == 0b00000000) { // change
                    colon = 0b10000000;
                    colCounter++;
                    }
                else {
                    colon = 0b00000000;
                }
    if(colCounter==1){

        if(secondOne<=9)
        {
            secondOne++;
        }
        colCounter=0;
    }


    // Call back after 1 second
        schdCallback(clockUpdate, time + 500);

}
void checkPushButton(uint32_t time){
    int code = pbRead();
        uint32_t delay;
        switch (code) {
        case 1:
            minuteOne++;
            delay = 250;             // Use an inertia for soft debouncing
            break;

        case 2:
           secondOne++;
           delay = 250; // Use an inertia for soft debouncing
           break;

            default:
            delay = 10;
        }
            schdCallback(checkPushButton, time + delay);
}
    int main(void){
    lpInit();
    seg7Init();



    uprintf("%s\n\r", "Lab 2: Wall clock");

    // Schedule the first callback events for LED flashing and push button checking.
    // Those trigger callback chains. The time unit is millisecond.
    schdCallback(clockUpdate, 1000);
    schdCallback(checkPushButton, 1005);
    // Loop forever
    while (true) {
        schdExecute();
    }
}


