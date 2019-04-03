;
;      ranger.asm
;
;      Created on: Apr 1, 2018
;      Author: arsalanbabar
;

      .cdecls "stdint.h", "stdbool.h", "stdio.h", "driverlib/pin_map.h", "inc/hw_memmap.h", "driverlib/sysctl.h", "launchpad.h", "ranger.h","driverlib/gpio.h", "driverlib/timer.h"

			.text

GPIO_PERIPHERAL		.field	SYSCTL_PERIPH_GPIOC
TIMER_PERIPHERAL	.field	SYSCTL_PERIPH_WTIMER0
TIMER_BASE			.field	WTIMER0_BASE
TIMER				.field  TIMER_A
TIMER_EVENT_CAP		.field  TIMER_CAPA_EVENT
GPIO_PIN			.field	GPIO_PIN_4
GPIO_PORT			.field  GPIO_PORTC_BASE
GPIO_TIMER_PIN		.field  GPIO_PC4_WT0CCP0

RangerInit		PUSH {LR}
				;We have to call SysCtlPeripheralEnable for GPIO Peripheral
				LDR 	r0, GPIO_PERIPHERAL
				BL		SysCtlPeripheralEnable

				;We have to call SysCtlPeripheralEnable for Timer peripheral
				LDR		r0, TIMER_PERIPHERAL
				BL		SysCtlPeripheralEnable

				LDR		r0, GPIO_PORT
				BL		SysCtlPeripheralEnable

				;For Time Configuration
				LDR		r0, TIMER_BASE
				MOV		r1, #TIMER_CFG_SPLIT_PAIR
				ORR		r1, #TIMER_CFG_A_CAP_TIME_UP
				BL		TimerConfigure

				;Call TimerControl Event
				LDR		r0, TIMER_BASE
				MOV		r1, #TIMER_A
				MOV		r2, #TIMER_EVENT_BOTH_EDGES

				;To enable Timer and Sub-Timer
				LDR		r0, TIMER_BASE
				LDR		r1, TIMER
				BL      TimerEnable

				POP   {PC}

rangerRead		PUSH {LR}
                ;GPIOPinTypeGPIOOutput(GPIO_PORTC_BASE, GPIO_PIN_4);
                LDR		r0, GPIO_PORT
                LDR		r1, GPIO_PIN
                BL		GPIOPinTypeGPIOOutput
				; GPIOPinWrite(GPIO_PORTC_BASE, GPIO_PIN_4, 0);
				LDR		r0, GPIO_PORT
				LDR     r1, GPIO_PIN
				MOV		r2, #0
				BL		GPIOPinWrite

			    ;low voltage for 2 mircroseconds
   				;waitUs(2)
				MOV		r0, #2
				BL		waitUs

				;//High Voltage for 5 microseconds
    			;GPIOPinWrite(GPIO_PORTC_BASE, GPIO_PIN_4, GPIO_PIN_4);
    			;waitUs(5);
    			LDR		r0, GPIO_PORT
    			LDR		r1, GPIO_PIN
    			LDR		r2, GPIO_PIN
    			BL		GPIOPinWrite

    			MOV		r0, #5
    			BL		waitUs

    			;low output
    			;GPIOPinWrite(GPIO_PORTC_BASE, GPIO_PIN_4, 0);
    			LDR		r0, GPIO_PORT
    			LDR		r1, GPIO_PIN
    			MOV		r2, #0
    			BL		GPIOPinWrite

    			;GPIOPinTypeTimer(GPIO_PORTC_BASE, GPIO_PIN_4);
    			LDR		r0, GPIO_PORT
    			LDR		r1, GPIO_PIN
    			BL		GPIOPinTypeTimer

    			;GPIOPinConfigure(GPIO_PC4_WT0CCP0);
    			LDR		r0, GPIO_TIMER_PIN
    			BL		GPIOPinConfigure

				;TimerIntClear(WTIMER0_BASE, TIMER_CAPA_EVENT);
				LDR		r0, TIMER_BASE
				LDR		r1, TIMER_EVENT_CAP
				BL		TimerIntClear

				; while(!TimerIntStatus(WTIMER0_BASE,false));
while_loop		LDR		r0, TIMER_BASE
				MOV		r1, #0
				BL		TimerIntStatus
				CMP 	r0, #0
				BEQ		while_loop


    			;uint32_t risetime = TimerValueGet(WTIMER0_BASE,TIMER_A);
                LDR		r0, TIMER_BASE
                LDR		r1, TIMER
                BL		TimerValueGet
                PUSH	{r0}
    			;TimerIntClear(WTIMER0_BASE, TIMER_CAPA_EVENT);
                LDR		r0, TIMER_BASE
				LDR		r1, TIMER_EVENT_CAP
				BL		TimerIntClear

			;Detect falltime
    		;while(!TimerIntStatus(WTIMER0_BASE, false));
    		;uint32_t falltime = TimerValueGet(WTIMER0_BASE, TIMER_A);
    		;TimerIntClear(WTIMER0_BASE, TIMER_CAPA_EVENT);
			;uint32_t difference = falltime - risetime;
			;return difference;

while_loop1		LDR		r0, TIMER_BASE
				MOV		r1, #0
				BL		TimerIntStatus
				CMP		r0, #0
				BEQ		while_loop1

				LDR		r0, TIMER_BASE
				LDR		r1, TIMER
				BL		TimerValueGet
				PUSH	{r0}

				;TimerIntClear(WTIMER0_BASE, TIMER_CAPA_EVENT);
                LDR		r0, TIMER_BASE
				LDR		r1, TIMER_EVENT_CAP
				BL		TimerIntClear

				POP {r1,r0}

				SUB r0, r0, r1 ;difference falltime-risetime

				POP  {pc}


