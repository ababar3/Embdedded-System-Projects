/*
 * main.c for ECE 266 Lab 3, stopwatch
 *  Created on: Aug 23, 2016
 *  ARSALAN BABAR & IMRAN BABAR
 *  Last updated on: Sep 19, 2017
 *      Author: zzhang
 */
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <inc/hw_memmap.h>
#include <driverlib/gpio.h>
#include <driverlib/pin_map.h>
#include <driverlib/sysctl.h>
#include "launchpad.h"
#include "seg7.h"
#include "seg7digit.h"
#include "ranger.h"

// The running state of the stopwatch system
enum {ON, OFF} sysState = OFF;
enum {inches, milli}  units = milli;

 //The initial state of the 7-segment display: "00:00" with colon on
 seg7Display_t seg7Display = {10, 10, 10, 0, 0};

void
displayMode(uint32_t time)
{

    uint32_t ranger = rangerRead();

    if (sysState == ON){
        if(units == milli) {
            ranger = ((ranger/2)* 340 / 50000);     //to calculate in millimeter
        }
        else {
            ranger = ((ranger/2)* 340 / 50000)* .0394;  //to calculate in Inches
        }
           seg7Display.d1 = ranger % 10;


           if (ranger < 10) {        //Update the display
               seg7Display.d2 = 10;
           }
           else {
               seg7Display.d2 = (ranger / 10) % 10;
           }

           if(ranger < 100){
               seg7Display.d3 = 10;   //Update the display
           }
           else{
               seg7Display.d3 = (ranger / 100) % 10;
           }

           if( ranger < 1000){
               seg7Display.d4 = 10;
           }
           else{
               seg7Display.d4 = (ranger / 1000) % 10;
           }

    }
    seg7DigitUpdate(&seg7Display);
    schdCallback(displayMode, time + 750);
 }


 //Callback function for checking push button. SW1 is the RESET button, and
 //SW2 is the START/PAUSE/RESUME button.
void
checkPushButton(uint32_t time)
{
    int code = pbRead();
    uint32_t delay;

    switch (code) {
    case 1:                          //SW1 is used to start and pause the ranger.
        if (sysState  == OFF)
        {
            sysState = ON;
        }
        else{
            sysState = OFF;
        }
        delay = 250;                 //software debouncing
        break;

    case 2:                          //SW2 is being used to switch between millimeter and inches
        if (units == milli){
            units = inches;
        }
        else{
            units = milli;
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
    RangerInit();

    // Update the clock display with initial values of 0
     seg7DigitUpdate(&seg7Display);

     //Schedule the first callback events for LED flashing and push button checking.
     schdCallback(checkPushButton, 1000);
     //Those trigger callback chains. The time unit is millisecond.
    schdCallback(displayMode, 255);

    uprintf("%s\n\r", "Lab 7: Ranger");
     //Run the event scheduler to process callback events
    while (true) {
        schdExecute();
    }
}
