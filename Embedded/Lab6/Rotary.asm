; Buzzer driver functions built on top of Tivaware
; Lab 4, ECE 266, spring 2018
; Created by Zhao Zhang

; To include names declared in C
 .cdecls "stdint.h", "stdbool.h", "stdio.h", "driverlib/pin_map.h", "driverlib/adc.h", "inc/hw_memmap.h", "driverlib/sysctl.h", "launchpad.h", "rotary.h","driverlib/gpio.h"
					.text
;					.global MotionOn
;					.global MotionOff

; GPIO peripheral, port base and pin mask value for the buzzer
; The buzzer is assumed to be connected to J17, and then the buzzer's
; signal pin is connected to PC4.
ROTARY_PERIPH     .field  SYSCTL_PERIPH_ADC0
ROTARY_PORT       .field  ADC0_BASE
ROTARYConfig      .field  ADC_TRIGGER_PROCESSOR ;Priority, TRIGGERS THE ADC SAMPLE SEQUENCE
ROTARYInterrupt   .field  ADC_CTL_IE  ; Set Interrupt Flag
ROTARYLast		  .field  ADC_CTL_END ; Mark the last step
ROTARYChannel	  .field  ADC_CTL_CH2 ;ADC CHANNEL 2
ROTARYF           .field  ADC_COMP_INT_NONE
;


ADCInit				 PUSH 	{LR}            ;For initializing the ADC

               			 LDR   r0, ROTARY_PERIPH
                		 BL    SysCtlPeripheralEnable   ;Branch linked to enable the peripheral

     					 LDR   r0, ROTARY_PORT
                		 MOV   r1, #0
                	     LDR   r2, ROTARYConfig
                		 MOV   r3, #0
               			 BL	   ADCSequenceConfigure  ;Configures ADC Sequencer

       					 LDR   r0, ROTARY_PORT
                		 MOV   r1, #0
                		 MOV   r2, #0
                		 LDR   r3, ROTARYInterrupt
                		 LDR   r4, ROTARYLast
                		 LDR   r5, ROTARYChannel
                		 ORR   r3, r3, r4 ; Orred Interrupt flag and rotary marks the end
                		 ORR   r3, r3, r5 ; Orred Interrupt flag with Channel 2.
                		 BL	   ADCSequenceStepConfigure

						 LDR r0, ROTARY_PORT
						 MOV r1, #0
						 BL  ADCSequenceEnable    ;Enables the first sample sequencer


                		 POP   {PC}

ADCRead          	     PUSH  {LR}                ;To read the raw value from ADC

               			 LDR   r0, ROTARY_PORT
               			 MOV   r1, #0
                		 BL    ADCProcessorTrigger ;Triggers the ADC Sample sequence

while_loop				 LDR  r0, ROTARY_PORT
						 MOV  r1, #0
						 MOV  r2, #0
						 BL   ADCIntStatus
						 CMP  r0, #0                     ;It would keep reading until r0 is not zero.
						 BEQ  while_loop


						 SUB  sp, #4                     ; save return address

						 LDR  r0, ROTARY_PORT            ; Loaded port in r0
						 MOV  r1, #0
						 MOV  r2, sp                     ; address of ADCRead is sp
						 BL   ADCSequenceDataGet         ;Gets the ADC Sequence Data

						 POP  {r0, pc}
