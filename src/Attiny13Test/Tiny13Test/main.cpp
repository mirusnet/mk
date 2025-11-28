/*
* Tiny13Test.cpp
*
* Created: 04-Feb-21 11:16:39 AM
* Author : mirus

* The device is shipped with CKSEL=10, SUT =10, and CKDIV8 programmed.
* The default clock source setting is therefore the Internal RC Oscillator running at 9.6 MHz with longest startup
* time and an initial system clock prescaling of 8.
*
*	TIMERS
*	T[period (sec)] = ((256 or 65536 [8/16 bit]) * Prescaler)) / F_CPU
*	Example T = (256*1024)/1000000 = 0.26
*	So that the timer will be overflowed every 0.26 sec, ~ 4Hz.
*
*
*/

//#define F_CPU 9600000UL
#define F_CPU 1200000UL


#include <avr/io.h>
#include <util/delay.h>
#include <avr/power.h>
#include <avr/sleep.h>
#include <avr/interrupt.h>

#define BIT_SET(a,b) ((a) |= (1<<(b)))
#define BIT_CLE(a,b) ((a) &= ~(1<<(b)))
#define BIT_FLIP(a,b) ((a) ^= (1<<(b)))
#define BIT_CHECK(a,b) ((a) & (1<<(b)))



#define LED_PIN PINB2
#define OUTPUT_PIN PINB4
#define INPUT_PIN PINB3




int main(void)
{
	clock_prescale_set(clock_div_8); // Lets run at 1.2 MHz
	

	BIT_SET(ACSR, ACD);					// Disable Analog Comparator
	BIT_CLE(ADCSRB, ACME);				// Disable Analog to Digital Converter

	BIT_SET(DDRB, LED_PIN);  // set LED_PIN as output
	BIT_CLE(PORTB, LED_PIN); // LED OFF
	
	BIT_CLE(DDRB, INPUT_PIN);   // input
	BIT_SET(DDRB, OUTPUT_PIN);  // output
	
	
		while (1)
		{
			if(BIT_CHECK(PINB, INPUT_PIN)) {
				
			}
			
			BIT_SET(PORTB, LED_PIN); // LED OFF
			_delay_ms(200);
			BIT_CLE(PORTB, LED_PIN); 
			_delay_ms(200);
			
		}
}

