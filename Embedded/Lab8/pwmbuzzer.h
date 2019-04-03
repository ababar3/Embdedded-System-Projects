/*
 * pwmbuzzer.h
 *
 *      Created on: Apr 9, 2018
 *          Author: arsalanbabar
 */

#ifndef PWMBUZZER_H_
#define PWMBUZZER_H_


 /** Initialize the timer PWM functions connected to the buzzer.
*/
void pwmSoundInit();

/*
 * Have to Set the frequency of the buzzer
 */
void pwmSoundFrequency(uint32_t freq, uint32_t dutyCycle);


#endif /* PWMBUZZER_H_ */
