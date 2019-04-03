/*
 * pwmbuzzer.c
 *Created on: Apr 9, 2018
 *      Author: arsalanbabar
 */

#include <stdint.h>
#include <stdbool.h>
#include <inc/hw_memmap.h>
#include <driverlib/sysctl.h>
#include <driverlib/gpio.h>
#include <driverlib/pin_map.h>
#include <driverlib/timer.h>
#include "pwmbuzzer.h"


void
pwmSoundInit()
{
    //This Enables the GPIO port peripheral
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOC);

    //This Would Enable Wide Timer 0
    SysCtlPeripheralEnable(SYSCTL_PERIPH_WTIMER0);

    //This Connects The Pins To The Timers
    GPIOPinTypeTimer(GPIO_PORTC_BASE, GPIO_PIN_4);
    GPIOPinConfigure(GPIO_PC4_WT0CCP0);

    // We would have to choose the PWM for Wide Timer 0 sub-Timer A
    TimerConfigure(WTIMER0_BASE, (TIMER_CFG_SPLIT_PAIR | TIMER_CFG_A_PWM));

    // To change the intensity level
    TimerLoadSet(WTIMER0_BASE, TIMER_A, 100);
    TimerMatchSet(WTIMER0_BASE, TIMER_A, 100-1);

    // This enables TimerA for Timer 0
    TimerEnable(WTIMER0_BASE, TIMER_A);
}

void
pwmSoundFrequency(uint32_t Period, uint32_t DutyCycle)
{

    TimerLoadSet(WTIMER0_BASE, TIMER_A, Period);  //Sets the value in load register period
    TimerMatchSet(WTIMER0_BASE, TIMER_A, Period - DutyCycle);
}


