/*
 * Arsalan Babar & Imran Babar
 *
 * Final Project
 *
 * main.c
 *
 *
 */
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <inc/hw_memmap.h>
#include <inc/hw_ints.h>
#include <driverlib/gpio.h>
#include <driverlib/pin_map.h>
#include <driverlib/sysctl.h>
#include <driverlib/systick.h>
#include <driverlib/adc.c>
#include <driverlib/adc.h>
#include "launchpad.h"
#include "motion.h"
#include "mic.h"



typedef enum {Off, On} switch_t;

static switch_t Motionstate = Off; //current state of motion is off
static switch_t Micstate = On;   //initially the Mic sensor will be on.


uint32_t delay; // to delay the led

void
SoundInput(uint32_t time)                         //Sound Sensor function to activate when push button SW2 is pressed
{

   uint32_t delay = 100;
    int input = InputVal();                        //created a sound variable to hold the value of the sound sensor

    if (Micstate == On){                       //If sound sensor detects sound it will turn on
    if (input>= 1000){                         //Sensitivity
     ledTurnOnOff(true, true, true);           //LED color white
               delay = 3000;                   //Delay of light for 3 seconds.
           }
           else {
               ledTurnOnOff(false, false, false);  //if no sound LED is off
           }
    }
    schdCallback(SoundInput, time + delay);
}


void
MotionDetector(uint32_t time)                          //For Motion Activation
{

   uint32_t delay = 100;

        if(Motionstate == On){                      //Checks for Motion Detection
         if(motionDetect()){                        //If motion is detected, LED TURNS ON
                ledTurnOnOff(true, true, true);     //LED color white
                delay = 3000;                       //Current led is delayed for 3 seconds

        }
        else {
            ledTurnOnOff(false, false, false);      //If nothing is detected, LED will stay off
        }

        }
     schdCallback(MotionDetector, time + delay);
}


void
checkPushButton(uint32_t time)
{
    int code = pbRead();
    uint32_t delay = 250;

    switch (code) {
    case 1:                         // SW1 is the case that it's day time and is only using the motion sensor

        uprintf("%s\n\r" ,"Motion Activation");
            Micstate = Off;      //Sound State is off since it's day time
        if(Motionstate == On){
            Motionstate = Off;
        }
        else{
            Motionstate = On;
        }
        delay = 250;
        break;

    case 2:                         // SW2 is the case where it's night time and is only using the sound sensor

        uprintf("%s\n\r" ,"Sound Activation");
        Motionstate = Off;     // Motion State is off since it's night time
        if(Micstate == On){
           Micstate = Off;
        }
        else{
            Micstate = On;
        }
        delay = 250;
        break;

    }
     schdCallback(checkPushButton, time + delay);
    }


int main(void){
    lpInit();
    motionInit();
    soundInit();

    uprintf("%s\n\r", "Final Project");


    schdCallback(checkPushButton, 1010);
    schdCallback(MotionDetector, 1015);
    schdCallback(SoundInput, 1030);

    while (true) {
        schdExecute();
    }
}
