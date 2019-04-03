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

int S2counter=0;
// Stop watch increment function, written in assembly
extern void
stopWatchIncrement(seg7Display_t* seg7Display);

// The running state of the stopwatch system
enum {
    Reset, Run, Pause
}  sysState = Run;

// The initial state of the 7-segment display: "00:00" with colon on
seg7Display_t seg7Display = {
    0, 0, 0, 0, 1
};

// Callback function for updating clock watch
void
stopWatchUpdate(uint32_t time)                          // The scheduled time
{
    // Update clock and display, only if the stopwatch is running
    if (sysState == Run) {
        stopWatchIncrement(&seg7Display);
    }

    seg7DigitUpdate(&seg7Display);

    // Call back after 10 milliseconds
    schdCallback(stopWatchUpdate, time + 10);
}

// Callback function for checking push button. S1 is the RESET button, and
// S2 is the START/PAUSE/RESUME button.
void
checkPushButton(uint32_t time)
{
    int code = pbRead();
    uint32_t delay;

    switch (code) {
    case 1:        // S1 is the Reset button, only when the stopwatch is paused

            if(sysState == Pause){
                seg7Display.d1=0;
                seg7Display.d2=0;
                seg7Display.d3=0;
                seg7Display.d4=0;
                sysState= Reset;
            }
        delay = 250;                // software debouncing
        break;

    case 2:                         // S2 is the Start/Pause/Resume button

            if(S2counter==0){
                sysState= Run;
                S2counter++;
            }
            else if(S2counter==1){
                sysState= Pause;
                S2counter=0;

            }



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

    uprintf("%s\n\r", "Lab 3: Stopwatch");

    // Update the clock display
    seg7DigitUpdate(&seg7Display);

    // Schedule the first callback events for LED flashing and push button checking.
    // Those trigger callback chains. The time unit is millisecond.
    schdCallback(stopWatchUpdate, 1000);
    schdCallback(checkPushButton, 1005);

    // Run the event scheduler to process callback events
    while (true) {
        schdExecute();
    }
}
