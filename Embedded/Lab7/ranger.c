/*
 * ranger.c
 * Author: arsalanbabar
 * Created on: Mar 26, 2018
 *
 */
#include <stdint.h>
#include <stdbool.h>
#include <driverlib/sysctl.h>
#include <inc/hw_memmap.h>
#include <driverlib/pin_map.h>
#include <driverlib/timer.h>
#include "launchpad.h"

void RangerInit(){
    // in order to configure timerA
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOC);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_WTIMER0);
    TimerConfigure(WTIMER0_BASE, (TIMER_CFG_SPLIT_PAIR | TIMER_CFG_A_CAP_TIME_UP));

    //configure timerB
    TimerControlEvent(WTIMER0_BASE, TIMER_A, TIMER_EVENT_BOTH_EDGES);
    TimerEnable(WTIMER0_BASE, TIMER_A);

}



uint32_t rangerRead(){


    GPIOPinTypeGPIOOutput(GPIO_PORTC_BASE, GPIO_PIN_4);
    GPIOPinWrite(GPIO_PORTC_BASE, GPIO_PIN_4, 0);
    //low voltage @ 2 mircroseconds
    waitUs(2);

    //High Voltage @ 5 microseconds
    GPIOPinWrite(GPIO_PORTC_BASE, GPIO_PIN_4, GPIO_PIN_4);
    waitUs(5);

    //output is low
    GPIOPinWrite(GPIO_PORTC_BASE, GPIO_PIN_4, 0);
    GPIOPinTypeTimer(GPIO_PORTC_BASE, GPIO_PIN_4);
    GPIOPinConfigure(GPIO_PC4_WT0CCP0);
    TimerIntClear(WTIMER0_BASE, TIMER_CAPA_EVENT);

    //To Detect Risetime
    while(!TimerIntStatus(WTIMER0_BASE,false));
    uint32_t risetime = TimerValueGet(WTIMER0_BASE,TIMER_A);
    TimerIntClear(WTIMER0_BASE, TIMER_CAPA_EVENT);

    //To Detect falltime
    while(!TimerIntStatus(WTIMER0_BASE, false));
    uint32_t falltime = TimerValueGet(WTIMER0_BASE, TIMER_A);
    TimerIntClear(WTIMER0_BASE, TIMER_CAPA_EVENT);


    uint32_t difference = falltime - risetime;

    return difference;

}

