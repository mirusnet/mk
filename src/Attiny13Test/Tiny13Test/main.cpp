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
#define IR_PIN PINB1
#define PC_INTERRUPT_PIN PINB4



//ISR(TIM0_OVF_vect)
//{
//BIT_FLIP(PORTB, LED_PIN);
//}

/*
BIT_CHECK(TIFR0,TOV0) <= this works either with:
1. disabled Timer Overflow interrupt
2. enabled Timer Overflow interrupt and ISR(TIM0_OVF_vect) configured.
*/
volatile uint8_t bit = 32;
volatile unsigned long data = 0;


#define LEADING_PULSE_START 58	// 12.37 ms
#define LEADING_PULSE_STOP 70	// 14.93 ms

#define REPEATE_CODE_START  48	// 10.24 ms
#define REPEATE_CODE_END  59	// 12.59 ms

#define IVALID_CODE_START  13	// 2.77  ms
#define VALID_CODE_START  8		// 1.7   ms



ISR(INT0_vect)
{

	uint8_t counter = TCNT0;
	int Overflow = BIT_CHECK(TIFR0,TOV0);
	
	
	if (bit == 32) {
		
		if ((counter >= LEADING_PULSE_START) && (counter <= LEADING_PULSE_STOP) && (Overflow == 0)) {           //12.26-14.82  Leading pulse burst 9ms && 4.5ms space
			
			data = 0; bit = 0;
			} else if ((counter >= REPEATE_CODE_START) && (counter <= REPEATE_CODE_END) && (Overflow == 0)) { //10.13-12.37  REPEAT CODE a 9ms leading pulse - 2.25ms space - 562.5µs pulse
			// do something on a repeating signal
		}
		
		} else {
		
		if ((counter > IVALID_CODE_START) ) {                              //2.77
			bit = 32; // Invalid - restart
			//BIT_FLIP(PORTB, LED_PIN);
			
		}
		else {
			if (counter > VALID_CODE_START) {                                                //1.6  HIGH 0
				data = data | ((unsigned long) 1<< (31-bit));         // the result will be inverted xxxx xxxx 11111 0000
				
			}
			
			if (bit == 31) {
				// data processing here, as we have a full dataset i.e. 32 bit
				if(data == 0xFFA25D) {
					BIT_SET(PORTB, LED_PIN);
					}else if(data == 0xFFE21D) {
					BIT_CLE(PORTB, LED_PIN);
				}
			}
			bit++;
		}
	}
	
	
	TCNT0 = 0;              // Clear counter
	BIT_SET(TIFR0,TOV0);    // Clear overflow flag :)
	BIT_SET(GIFR, INTF0);   // Clear INT0 flag
	
}

// Button pressing
ISR(PCINT0_vect) {
	
if(!BIT_CHECK(PINB, PC_INTERRUPT_PIN)) {

	if(BIT_CHECK(PINB, LED_PIN)) {
		BIT_CLE(PORTB, LED_PIN);
	} else {
		BIT_SET(PORTB, LED_PIN);
	}
	
	_delay_ms(300);
	
	}
}


int main(void)
{
	clock_prescale_set(clock_div_8); // Lets run at 1.2 MHz
	

	BIT_SET(ACSR, ACD);					// Disable Analog Comparator
	BIT_CLE(ADCSRB, ACME);				// Disable Analog to Digital Converter

	BIT_SET(DDRB, LED_PIN);  // set LED_PIN as output
	BIT_CLE(DDRB, IR_PIN);   // set IR PIN as input
	BIT_SET(PORTB, IR_PIN);  // pull up res for IR PIN
	
	//BIT_SET(TCCR0B, CS02);  // set timer prescaler to 1024 (if CLK=1200000Hz/1024/256 = ~4.58Hz, =~0.22s overflow) One tick period is ~853mk seconds
	//BIT_SET(TCCR0B, CS00);  // set timer prescaler to 1024 (if CLK=9600000Hz/1024/256 = ~37Hz,   =~27ms  overflow) One tick period is ~107mk seconds
	
	
	BIT_CLE(TCCR0B, CS00);   // timer prescaler = 256
	BIT_CLE(TCCR0B, CS01);
	BIT_SET(TCCR0B, CS02);
	
	BIT_SET(GIMSK, INT0);  // enable interrupt on INT0
	BIT_SET(MCUCR, ISC01); // falling edge on INT0
	BIT_CLE(MCUCR, ISC00);
	
	
	BIT_SET(GIMSK, PCIE);				// enable interrupt PIN CHANGE INTERRUPT
	BIT_SET(PCMSK, PCINT4);				// enable interrupt PIN CHANGE INTERRUPT
	BIT_CLE(DDRB, PC_INTERRUPT_PIN);    // set BUTTON as input
	BIT_SET(PORTB, PC_INTERRUPT_PIN);   // pull up res for BUTTON PIN
	
	//BIT_SET(TIMSK0,TOIE0); // enable Timer Overflow interrupt
	set_sleep_mode(SLEEP_MODE_IDLE);
	sei(); // enable global interrupts
	
	BIT_CLE(PORTB, LED_PIN); // LED OFF
	
	while (1)
	{
		
		cli();							// Disable Interrupts
		sleep_enable();					// Enable Sleep Mode
		sleep_bod_disable();			// Disable the Brown Out Detector (during sleep)
		sei();							// Enable Interrupts
		sleep_cpu();					// Go to Sleep
		sleep_disable();				// Entrance point
		_delay_ms(3000);
	}
}


/*
int main(void)
{
	clock_prescale_set(clock_div_8); // Lets run at 1.2 MHz
	

	BIT_SET(ACSR, ACD);					// Disable Analog Comparator
	BIT_CLE(ADCSRB, ACME);				// Disable Analog to Digital Converter

	BIT_SET(DDRB, LED_PIN);  // set LED_PIN as output
	BIT_CLE(PORTB, LED_PIN); // LED OFF
	
		while (1)
		{
			
			BIT_SET(PORTB, LED_PIN); // LED OFF
			_delay_ms(500);
			BIT_CLE(PORTB, LED_PIN); 
			_delay_ms(500);
			
		}
}
*/
