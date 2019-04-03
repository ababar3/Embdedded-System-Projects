;
;      mic.asm
;
;      Created on: Apr 25, 2018
;      Author: arsalanbabar


 		.cdecls "stdint.h", "stdbool.h", "driverlib/sysctl.h", "inc/hw_memmap.h", "driverlib/gpio.h", "driverlib/pin_map.h", "launchpad.h", "mic.h", "driverlib/adc.h"

 		.text

SYS_PERIPHERAL		.field	SYSCTL_PERIPH_ADC0
BASE_PERIPHERAL		.field	ADC0_BASE
TRIGGER_PROCESSOR	.field	ADC_TRIGGER_PROCESSOR

				.global soundInit
soundInit		PUSH {LR}
				;Enables ADC0 module
				;SysCtlPeripheralEnable(SYSCTL_PERIPH_ADC0);
				LDR		r0, SYS_PERIPHERAL
				BL		SysCtlPeripheralEnable
				;Wait for the ADC0 module to be ready
				; while(!SysCtlPeripheralReady(SYSCTL_PERIPH_ADC0))
while_loop		LDR		r0,	SYS_PERIPHERAL
				BL		SysCtlPeripheralReady
				CMP 	r0, #0
				BEQ		while_loop
				;Enables the first sample sequencer
				;ADCSequenceConfigure(ADC0_BASE, 0, ADC_TRIGGER_PROCESSOR, 0);
				LDR		r0,	BASE_PERIPHERAL
				MOV		r1, #0
				LDR		r2, TRIGGER_PROCESSOR
				MOV		r3, #0
				BL		ADCSequenceConfigure

				;ADCSequenceStepConfigure(ADC0_BASE, 0, 0, ADC_CTL_IE | ADC_CTL_END | ADC_CTL_CH2);
			    LDR		r0,	BASE_PERIPHERAL
			    MOV		r1, #0
			    MOV		r2, #0
			    MOV		r3, #ADC_CTL_IE
			    ORR		r3, #ADC_CTL_END
			    ORR		r3, #ADC_CTL_CH2
			    BL		ADCSequenceStepConfigure

			    ;ADCSequenceEnable(ADC0_BASE, 0);
			    LDR		r0, BASE_PERIPHERAL
			    MOV		r1, #0
			    BL		ADCSequenceEnable

			    POP   {PC}

InputVal		PUSH  {LR}

				;Triggers the sample sequence.
			    ;ADCProcessorTrigger(ADC0_BASE, 0);
			    LDR		r0, BASE_PERIPHERAL
			    MOV		r1, #0
			    BL		ADCProcessorTrigger

				;Waits until sample sequence is complete
			  	;while(!ADCIntStatus(ADC0_BASE, 0, false)){
while_loop1		LDR		r0, BASE_PERIPHERAL
				MOV		r1, #0
				MOV		r2, #0
				BL		ADCIntStatus
				CMP 	r0, #0
				BEQ		while_loop1

				;ADCSequenceDataGet(ADC0_BASE, 0, &ui32value);
				;return (uint16_t) ui32value;
				SUB		sp, #4
				LDR		r0, BASE_PERIPHERAL
				MOV		r1, #0
				MOV     r2, sp
				BL 	    ADCSequenceDataGet
				LDR		r0, [sp]
				ADD		sp, #4
				POP 	{PC}

