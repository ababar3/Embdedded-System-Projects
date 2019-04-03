;
;   pwmbuzzer.asm
;
;       Created on: Apr 9, 2018
;          Author: arsalanbabar
;
                .cdecls "stdint.h","stdbool.h","inc/hw_memmap.h","inc/hw_ints.h","driverlib/sysctl.h","driverlib/gpio.h","driverlib/pin_map.h","driverlib/timer.h","launchpad.h","pwmbuzzer.h"
                .text

TIMER_PERIPHERAL    .field  SYSCTL_PERIPH_WTIMER0
TIMERBASE0          .field  WTIMER0_BASE
PORT_PERIPHERAL     .field  SYSCTL_PERIPH_GPIOC
PIN4                .field  GPIO_PC4_WT0CCP0
GPIO_PORT            .field  GPIO_PORTC_BASE
TIMER		        .field	TIMER_A

                .asmfunc
pwmSoundInit      PUSH    {lr}

                ;SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOC);
                LDR     r0, PORT_PERIPHERAL
                BL      SysCtlPeripheralEnable

                ;SysCtlPeripheralEnable(SYSCTL_PERIPH_WTIMER0);
                LDR     r0, TIMER_PERIPHERAL
                BL      SysCtlPeripheralEnable

 				;GPIOPinTypeTimer(GPIO_PORTC_BASE, GPIO_PIN_4);
                LDR     r0, GPIO_PORT
                MOV     r1, #(GPIO_PIN_4)
                BL      GPIOPinTypeTimer

 				;GPIOPinConfigure(GPIO_PC4_WT0CCP0);
                LDR     r0, PIN4
                BL      GPIOPinConfigure ; double check

         		;TimerConfigure(WTIMER0_BASE, (TIMER_CFG_SPLIT_PAIR | TIMER_CFG_A_PWM));
                LDR     r0, TIMERBASE0
                MOV     r1, #TIMER_CFG_SPLIT_PAIR
                MOV     r2, #TIMER_CFG_A_PWM
                ORR     r1, r2
                BL      TimerConfigure


 				;TimerLoadSet(WTIMER0_BASE, TIMER_A, 100);
                LDR     r0,TIMERBASE0
                LDR     r1, TIMER
                MOV     r2, #100
                BL      TimerLoadSet

				;TimerMatchSet(WTIMER0_BASE, TIMER_A, 100-1);
                LDR     r0, TIMERBASE0
                LDR     r1, TIMER
                MOV     r2, #99
                BL      TimerMatchSet


				;TimerEnable(WTIMER0_BASE, TIMER_A);
                LDR     r0, TIMERBASE0
                LDR     r1, TIMER
                BL      TimerEnable

                POP     {pc}
                .endasmfunc


pwmSoundFrequency .asmfunc
				PUSH    {lr}


				;pwmSoundFrequency(uint32_t freq, uint32_t dutyCycle)
                MOV	r2, r0
                MOV r3, r1
                ;TimerLoadSet(WTIMER0_BASE, TIMER_A, freq);
                LDR r0, TIMERBASE0
                LDR r1, TIMER


                BL  TimerLoadSet
        	   ;TimerMatchSet(WTIMER0_BASE, TIMER_A, freq - dutyCycle);

                LDR r0, TIMERBASE0
                LDR r1, TIMER
				SUB r2, r3

                BL  TimerMatchSet

                POP     {pc}
                .endasmfunc

