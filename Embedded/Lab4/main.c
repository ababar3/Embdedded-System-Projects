/*
 * ECE 266, Spring 2018, Lab 4 Buzzer
 *Imran Babar and Arsalan Babar
 * main.c
 */

#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <inc/hw_memmap.h>
#include <driverlib/gpio.h>
#include <driverlib/pin_map.h>
#include <driverlib/sysctl.h>
#include "launchpad.h"
#include "buzzer.h"
#include "motion.h"

// Switch type
typedef enum {
    Off, On
} switch_t;

// The states of buzz playing: the state of the buzzer system, the state of sound, the state of the
// buzzer, and time left in the current sound state in millisecond
static switch_t buzzerSysState = Off;
static switch_t soundState = Off;
static switch_t buzzerState = Off;
static uint16_t timeLeft = 0;
static switch_t motionSysState = Off;

/*
 * Buzzer play callback function. It plays a buzz sound for 0.3 second, then be silent for 3 seconds.
 * In the buzz phase, the buzzer is turned on and off every 2 ms.  In the silent phase, the buzzer is turned
 * off all the time. The buzzer itself has a built-in resonant frequency of about 2300Hz (+/- 300Hz) when
 * turned on.
 *
 * The function also checks buzzerSysState. If the state is off, then it turns of the buzzer and
 * reset all the states.
 */
void
buzzerPlay(uint32_t time)
{
    uint32_t delay = 1;

    if (buzzerSysState == On && motionSysState == On) {

        switch (soundState) {
        case Off:
            if (timeLeft < delay) {              // switch to sound-on state
                buzzerOn();
                soundState = On;
                buzzerState = On;
                timeLeft = 300;
            }
            else {
                timeLeft -= delay;
            }
            break;

        case On:
            if (timeLeft < delay) {              // switch to sound-off state
                buzzerOff();
                soundState = Off;
                buzzerState = Off;
                timeLeft = 3000;
            }
            else {
                timeLeft -= delay;
                switch (buzzerState) {           // if buzzer is on, turn it off
                case Off:
                    buzzerOn();
                    buzzerState = On;
                    break;

                case On:                        // if buzzer is off, turn it on
                    buzzerOff();
                    buzzerState = Off;
                    break;
                }
            }

        }
        schdCallback(buzzerPlay, time + delay);    // call back in 2 ms
    }
    else {                                      // turn the buzzer system off
        buzzerOff();
        soundState = Off;
        buzzerState = Off;
        timeLeft = 0;
    }
}

//*********************************************************************
void
motionCheck(uint32_t time)
{
    bool sense = motionOn();
    uint32_t delay;
    if(buzzerSysState == On){
    switch (sense) {
    case 0:
        delay = 1500;
        motionSysState = Off;
        uprintf("Human motion not detected\n\r"); // this will constantly check that motion has not been detected here.
        break;

    default:
        delay = 1500;
        motionSysState = On; // If the motionState is true(detected) it will print the message that motion detected
        uprintf("Human motion detected\n\r");
        schdCallback(buzzerPlay, time + 1);     // schedule a callback to buzzerPlay()
        break;
    }
    }
    schdCallback(motionCheck, time + delay);
}
//*******************************************
/*
 * Event driven code for checking push button
 */
void
checkPushButton(uint32_t time)
{
    int code = pbRead();
    uint32_t delay;

    switch (code) {
    case 1:                                     // SW1: Start the buzzer
        if (buzzerSysState == Off) {
            buzzerSysState = On;
            delay = 250;
            uprintf("Motion is turned ON\n");  //Prints out when Motion is on (sw1 is pressed)
            schdCallback(motionCheck, time + delay); //Checks for any human motion
        }
        break;

    case 2:                                     // SW2: Stop the buzzer
        if (buzzerSysState == On) {
            buzzerSysState = Off;   // turned of buzzer if it was detected as on.
            delay = 250;
            uprintf("Motion is turned off\n");
        }
        break;

    default:
        delay = 10;
    }

    schdCallback(checkPushButton, time + delay);
}
//**********************************************************************************
int main(void)
{
    lpInit();
    buzzerInit();
    motionInit();

    uprintf("%s\n\r", "Lab 4 starts");

    schdCallback(checkPushButton, 1010);
    schdCallback(motionCheck, 1010);
    while (true) {
        schdExecute();
    }
}
