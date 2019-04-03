/*
 * main.c: For Lab 5, Sleeping LaunchPad
 *Arsalan Babar And Imran Babar
 * Created by Zhao Zhang.
 */

#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <inc/hw_memmap.h>
#include <inc/hw_ints.h>
#include <driverlib/gpio.h>
#include <driverlib/pin_map.h>
#include <driverlib/sysctl.h>
#include <driverlib/interrupt.h>
#include <driverlib/systick.h>
#include "launchpad.h"
#include "motion.h"

// Color defines
#define     RED         0
#define     BLUE        1
#define     GREEN       2

// Color display setting
static volatile int colorSetting = BLUE;
static bool colorTable[3][3] = {
    {true, false, false},       // Red
    {false, true, false},       // Blue
    {false, false, true}        // Green
};

/*
 * LED flashing function. Note that this function must run in a thread, e.g. not be callback-based,
 * because the frequency of buzzer on/off is too high for the callback scheduling module.
 */
void
ledFlash()
{
    int i;

    while (true) {
            IntMasterDisable();     // disable all interrupts so that the color stays the same

            for (i = 0; i < 3; i++) {
                ledTurnOnOff(colorTable[colorSetting][0], colorTable[colorSetting][1],
                             colorTable[colorSetting][2]);
            waitMs(300);
            ledTurnOnOff(false, false, false);
            waitMs(300);
        }

            IntMasterEnable();      // enable interrupt signal from interrupt controller to CPU

            // The following is in-line assembly, i.e. inserting assembly instructions into C code
            // Execute an WFI instruction to put the processor into the sleep mode
            __asm("    wfi");
    }
}

/*
 * Interrupt handler for push button 1 (pins PF0 and PF4)
 */
void
pbIntrHandler()
{
    // Clear interrupt. This is necessary, otherwise the interrupt handler will be executed forever.
    /**************************************/
    int sense = pbRead();  // reads which button was pushed.
    GPIOIntClear(GPIO_PORTF_BASE, GPIO_PIN_0 | GPIO_PIN_4);

    // Set LED color to green
    /**************************************/
    if (sense == 2){
        colorSetting = BLUE;   // if sw2 is pushed blue led will flash
    }
    else if(sense == 1){
        colorSetting = GREEN; // if sw1 is pushed green led will flash
    }

}


/*
 * Interrupt handler for motion (port C; pin 5)
 */
/**************************************/
void
motionIntrHandler()
{
    // Clear interrupt. This is necessary, otherwise the interrupt handler will be executed forever.
    GPIOIntClear(GPIO_PORTC_BASE, GPIO_PIN_5);

    // Set LED color to red
    colorSetting = RED;            // On human motion detection, led will flash red.
}
/*
 * Select a set of interrupts that can wake up the LaunchPad
 */
void
setInterrupts()
{
    // Disable the system tick interrupt, which has been enabled in lpInit();
    // otherwise, it will wake up Tiva C.
    SysTickIntDisable();


    // Set interrupt on Port F, pin 0 (SW1) and pin 4 (SW2)
    GPIOIntRegister(GPIO_PORTF_BASE, pbIntrHandler);            // register the interrupt handler
    GPIOIntEnable(GPIO_PORTF_BASE, GPIO_PIN_0 | GPIO_PIN_4);    // enable interrupts on SW1 and SW2 input
    GPIOIntTypeSet(GPIO_PORTF_BASE, GPIO_PIN_0 | GPIO_PIN_4,    // interrupt on falling edge, note that SW1 and SW2 are active low
            GPIO_FALLING_EDGE);
    IntPrioritySet(INT_GPIOF, 0);                           // raise interrupt level to 0 (highest) to make sure MCU wakes up
/**************************************/
    //  Set interrupt on Port C, pin 5 For motion handler
    GPIOIntRegister(GPIO_PORTC_BASE, motionIntrHandler);            // register the interrupt handler
    GPIOIntEnable(GPIO_PORTC_BASE, GPIO_PIN_5);    // enable interrupts on SW1 and SW2 input
    GPIOIntTypeSet(GPIO_PORTC_BASE, GPIO_PIN_5,    // interrupt on falling edge, note that SW1 and SW2 are active low
                   GPIO_FALLING_EDGE);
    IntPrioritySet(INT_GPIOC, 0);                 // raise interrupt level to 0 (highest) to make sure MCU wakes up

}

int
main(void)
{
    // Initialize the launchpad, buzzer
    lpInit();

    // IN YOUR SOLUTION sense, call the function that initializes the PIR montion sensor,
    //    which you wrote in Lab 4, e.g.
    // pirInit();
    ////////////////////////////
    motionInit();   //initializes the motion sensor
    // NOTE: In this lab, you should NOT use the callback scheduler because it will wake up the CPU
    // from sleeping.

    setInterrupts();

    uprintf("%s\n\r", "Lab 5: Sleeping LaunchPad");

    ledFlash();
}
