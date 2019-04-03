
/*
 * main.c for Lab 8, spring 2018
 *
 * Created by Zhao Zhang
 * ARSALAN BABAR AND IMRAN BABAR
 */

#include <pwmled.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include "launchpad.h"
#include <pwmbuzzer.h>

static enum {Run, Pause} sysState = Pause;
static enum {ON, OFF}    soundState = OFF;


typedef struct {
    pwm_t pwm;              // PWM parameters
    uint16_t duration;      // Duration of this PWM segment
} pwmSeg_t;


pwmSeg_t pwmTable [] = {
                        {{1, 200}, 100},
                        {{2, 200}, 100},
                        {{3, 200}, 100},
                        {{4, 200}, 100},
                        {{5, 200}, 100},
                        {{6, 200}, 100},
                        {{7, 200}, 100},
                        {{8, 200}, 100},
                        {{9, 200}, 100},
                        {{10,200},200},
                        {{9, 200}, 100},
                        {{8, 200}, 100},
                        {{7, 200}, 100},
                        {{6, 200}, 100},
                        {{5, 200}, 100},
                        {{4, 200}, 100},
                        {{3, 200}, 100},
                        {{2, 200}, 100},
                        {{1, 200}, 100}
                        };
typedef struct {

    uint32_t pwmPeriod; //    Frequency of notes
    uint32_t pwmDutyCycle;  // Volume Control for buzzer
}   pwm_sound;


pwm_sound Frequency_Table [] =
                            {
                          //Music Notes for London Bridge
                          {127551, 1000},  //G4
                          {143266, 1000},  //F4
                          {113636, 1000},  //A4
                          {170068, 1000},  //D4
                          {151515, 1000},  //E4
                          {190840, 1000},  //C4
                          {190840, 1000},
                          {135135, 10000},
                          {227227, 10000},
                          {120481, 10000},
                          {180505, 10000},
                          {107296, 10000},
                          {101214, 10000},
                          {255102, 10000},
                          {202429, 10000},
                          {285714, 10000},
                          {100, 1} //this would produce no sound
                        };

#define LEGS    (sizeof(pwmTable)/sizeof(pwmTable[0]))


// Music notes as define above
int notes [] = {1,3,1,2,5,2,1,17,4,5,2,17,5,2,1,17,1,3,1,2,5,2,1,17,4,1,5,6,17,1,3,1,2,5,2};

/*
 * Check the push button. SW1 is the RUN key, SW2 is the PAUSE key
 */
void
checkPushButton(uint32_t time)
{
    int code = pbRead();
    uint32_t delay = 10;

    switch (code) {
        case 1:             // SW1 is used to Run and Pause the breath light.
            if (sysState == Pause){
                sysState = Run;
            }
            else {
                sysState = Pause;
            }
            delay = 250;
            break;

        case 2:             // SW2 is used to turn on and off buzzer.
            if (soundState == OFF){
                soundState = ON;
            }
            else {
                soundState = OFF;
            }

            delay = 250;
            break;
        }

        schdCallback(checkPushButton, time + delay);
}

/*
 * Change LED intensity level.
 */

void
pwmledUpdate(uint32_t time)
{
    static enum color_t {RED = 0, GREEN = 1, BLUE = 2} color = RED;     // Current color to display
    static int seg = 0;                                                 // Current segment of PWM settings

    if (sysState == Run) {
        // Set up PWM parameters for the three sub-LEDs. Only one LED is lit at a time.
        pwm_t off = {0, 200};
        pwm_t red = (color == RED) ? pwmTable[seg].pwm : off;
        pwm_t green = (color == GREEN) ? pwmTable[seg].pwm : off;
        pwm_t blue = (color == BLUE) ? pwmTable[seg].pwm : off;
        pwmledSetColor(red, green, blue);

        // Move to the next segment, switch color on warp-around
        seg = (seg + 1) % LEGS;
        if (seg == 0) {
            color = (enum color_t) ((color + 1) % 3);
        }

        schdCallback(pwmledUpdate, time + pwmTable[seg].duration);
    }
    else {
        // Check again after 10 ms
        schdCallback(pwmledUpdate, time + 10);
    }
}

void
pwmSongUpdate(uint32_t time)
{
    static int i = 0;

    // Here we schedule the next callback with delay before hand. Steady Delay
    schdCallback(pwmSongUpdate, time + 250);

    // Return immediately if system is paused
    if (soundState == OFF)
    {
        pwmSoundFrequency(Frequency_Table[16].pwmPeriod,Frequency_Table[16].pwmDutyCycle ); // When sound is off, the sound would mute from the buzzer (index 16 is mute)
        return;
    }
    // Cycle through the notes of the song
    //
    pwmSoundFrequency(Frequency_Table[notes[i]-1].pwmPeriod,Frequency_Table[notes[i]-1].pwmDutyCycle);  //pwmSoundFrequency is defined in assembly..
    if(i == 30)
    {
        i = 0;
    }
    else
        i++;
}
void main(void)
{
    lpInit();
    pwmledInit();
    pwmSoundInit();


    // Schedule the first callback events for LED flashing and push button checking.
    // Those trigger callback chains. The time unit is millisecond.
    schdCallback(checkPushButton, 1005);
    schdCallback(pwmledUpdate, 1010);

    schdCallback(pwmSongUpdate, 1020);
    uprintf("%s\n\r", "Lab 8: Music Player");

    // Loop forever
    while (true) {
        schdExecute();
    }
}




