; Buzzer driver functions built on top of Tivaware
; Lab 4, ECE 266, spring 2018
; Created by Zhao Zhang

; To include names declared in C
 .cdecls "stdint.h", "stdbool.h", "stdio.h", "inc/hw_memmap.h", "driverlib/pin_map.h", "driverlib/gpio.h", "driverlib/sysctl.h", "launchpad.h", "buzzer.h", "motion.h"

					.text
;					.global motionOn
;					.global motionOff

; GPIO peripheral, port base and pin mask value for the buzzer
; The buzzer is assumed to be connected to J17, and then the buzzer's
; signal pin is connected to PC4.
MOTION_PERIPH   .field  SYSCTL_PERIPH_GPIOC
MOTION_PORT     .field 	GPIO_PORTC_BASE
MOTION_PIN      .equ	GPIO_PIN_5


;
; void buzzInit(): Initialze the buzz peripheral, port, and pin direction
;
motionInit      PUSH 	{LR}

                ; Initialize the GPIO peripheral for the port that the buzzer uses:
                ;   Call SysCtlPeripheralEnable(BUZZER_PERIPH)
                LDR   r0, MOTION_PERIPH
                BL    SysCtlPeripheralEnable

                ; Set the GPIO pin for the buzzer as output type:
                ;   Call GPIOPinTypeGPIOOutput(BUZZER_PORT, BUZZER_PIN)
                LDR   r0, MOTION_PORT
                MOV   r1, #MOTION_PIN
                BL    GPIOPinTypeGPIOInput

                POP   {PC}

;
; void buzzOn(): Turn on the buzzer. It calls GPIOPinWrite() to write 1 to the signal pin.
;
motionOn        PUSH  {LR}

                ; Write 1 to the GPIO pin that the buzzer uses:
		;   Call GPIOPinWrite(BUZZ_PORT, BUZZ_PIN, BUZZ_PIN)
                LDR   r0, MOTION_PORT
                MOV   r1, #MOTION_PIN
                MOV   r2, #MOTION_PIN
                BL    GPIOPinRead

                POP   {PC}



