/*
* Attiny2313NightLight.c
*
* Created: 27/07/2017 13:08:28
* Author : rmikhaylenko
*/

#define F_CPU 250000


#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/power.h>
#include <util/delay.h>
#include <avr/sleep.h>
#include <stdbool.h>

#include "binary.h"

#define BIT_SET(a,b) ((a) |= (1<<(b)))
#define BIT_CLEAR(a,b) ((a) &= ~(1<<(b)))
#define BIT_FLIP(a,b) ((a) ^= (1<<(b)))
#define BIT_CHECK(a,b) ((a) & (1<<(b)))


#define LED_COUNT_NONE 0
#define LED_COUNT_ONE 1
#define LED_COUNT_TWO 2
#define LED_COUNT_FOUR 3
uint8_t default_led_count = LED_COUNT_TWO;


#define INDICATOR_NONE 0
#define INDICATOR_ONE 1
#define INDICATOR_TWO 2
#define INDICATOR_THREE 3
uint8_t default_indicator_count = INDICATOR_ONE;


volatile bool go_to_sleep = false;
volatile bool indicators_state = false;
volatile bool is_blinking = false;
volatile uint8_t iteration_8_bit_timer = 0;

void enable_indicators() {
		BIT_SET(PORTD, PIND6);
		BIT_SET(PORTD, PIND1);
		BIT_SET(PORTD, PIND5);
}
void disable_indicators() {
		BIT_CLEAR(PORTD, PIND6);
		BIT_CLEAR(PORTD, PIND1);
		BIT_CLEAR(PORTD, PIND5);
}


void set_led_count(uint8_t count) {
	if (count == LED_COUNT_ONE)	{
		BIT_SET(PORTB, PINB3);
		BIT_CLEAR(PORTB, PINB0);
		BIT_CLEAR(PORTB, PINB1);
		BIT_CLEAR(PORTB, PINB2);
		BIT_CLEAR(PORTB, PINB4);
		BIT_CLEAR(PORTB, PINB5);
		BIT_CLEAR(PORTB, PINB6);
	}
	else if (count == LED_COUNT_TWO) 	{
		BIT_SET(PORTB, PINB1);
		BIT_SET(PORTB, PINB5);
		BIT_CLEAR(PORTB, PINB0);
		BIT_CLEAR(PORTB, PINB2);
		BIT_CLEAR(PORTB, PINB3);
		BIT_CLEAR(PORTB, PINB4);
		BIT_CLEAR(PORTB, PINB6);
		
	}
	else if (count == LED_COUNT_FOUR) {
		BIT_SET(PORTB, PINB0);
		BIT_SET(PORTB, PINB2);
		BIT_SET(PORTB, PINB4);
		BIT_SET(PORTB, PINB6);
		BIT_CLEAR(PORTB, PINB1);
		BIT_CLEAR(PORTB, PINB3);
		BIT_CLEAR(PORTB, PINB5);
	}
	else if (count == LED_COUNT_NONE) {
		BIT_CLEAR(PORTB, PINB0);
		BIT_CLEAR(PORTB, PINB1);
		BIT_CLEAR(PORTB, PINB2);
		BIT_CLEAR(PORTB, PINB3);
		BIT_CLEAR(PORTB, PINB4);
		BIT_CLEAR(PORTB, PINB5);
		BIT_CLEAR(PORTB, PINB6);
	}
}

void adjust_led_count() {
	default_led_count >= 3 ? default_led_count = 1 : ++default_led_count;
}

void start_8_bit_timer() {
	TCCR0B	= (1<<CS00) | (1<<CS02); 		// Set up timer at F_CPU/1024 (Overflow every 8.4 sec at clock_div_256)
	TCNT0	= 0x00; 		 				// Zero timer (start it)
	iteration_8_bit_timer = 0;
}

void stop_8_bit_timer() {
	TCCR0B &= 0B11111000;					// Zero prescaler
	iteration_8_bit_timer = 0;
}

/*
BIT_SET(TIMSK, TOIE1);	// Enable overflow interrupt by timer T1

void start_16_bit_timer() {
	TCCR1B	= (1<<CS10) | (1<<CS12);		// Set up timer at F_CPU/1024
	TCNT1	= 0x00; 		 				// Zero timer (start it)
}

void stop_16_bit_timer() {
	TCCR1B &= 0B11111000;					// Zero prescaler
}

ISR(TIMER1_OVF_vect) {
}

*/


/************************************************************************/
/*	Button processing by INT1 interrupt GLOBAL ON/OFF MODE				*/
/************************************************************************/
ISR(INT1_vect) {
	if(go_to_sleep == false) {					// pressing on working mode
		set_led_count(LED_COUNT_NONE);			// Disable LEDs			** the same done in main, here is to appear fast
		disable_indicators();
		stop_8_bit_timer();						// If it was working at the moment
		go_to_sleep = true;
		is_blinking = false;					// If it was blinking
	} else {									// waiking up from sleep
		set_led_count(default_led_count);		// Restore turned on LEDs	** the same done in main, here is to appear fast
		go_to_sleep = false;
	}
	_delay_ms(400);
}


volatile bool schmitt_state = false;

void schmitt_on() {
	schmitt_state = true;
	BIT_SET(DDRB, PINB7);					// Set SCHMITT_TRIGGER pin as output
	BIT_SET(PORTB, PINB7);
	
}
void schmitt_off() {
	schmitt_state = false;
	BIT_CLEAR(DDRB, PINB7);					// Set SCHMITT_TRIGGER pin as input (Bug in PBC layot)
	BIT_CLEAR(PORTB, PINB7);				// Disable pull UP that was set by st_on
}

/************************************************************************/
/*	Button processing by INT0 interrupt  SCHMITT_TRIGGER ON/OFF			*/
/************************************************************************/
ISR(INT0_vect) {
	schmitt_state ? schmitt_off() : schmitt_on();
	_delay_ms(400);
}

void blink() {
	set_led_count(LED_COUNT_NONE);
	_delay_ms(15);
	set_led_count(default_led_count);
}


/************************************************************************/
/*	TIMER 0 8 BIT OVERFLOW ROUTINE			*/
/************************************************************************/
ISR(TIMER0_OVF_vect) {
	if(++iteration_8_bit_timer >90) { 
		is_blinking = true;
		blink();
	} 
	
	if(iteration_8_bit_timer >100) { 
		go_to_sleep = true;
		is_blinking = false;
		stop_8_bit_timer(); //stop itself
	}
}


int main(void)
{
	clock_prescale_set(clock_div_32);		//8MHz/32 = 250K
	
	//8 Bit timer. Overflow routine  - ISR(TIMER0_OVF_vect)
	BIT_SET(TIMSK, TOIE0);	// Enable overflow interrupt by timer T0
	start_8_bit_timer();



	ACSR|=(1<<ACD);							//Disable Analog Comparator

	DDRB	= 0xFF; 						// Set all pins of PORTB as output
	BIT_CLEAR(DDRB, PINB7);					// Set SCHMITT_TRIGGER pin as input (Bug in PBC layot)


	BIT_CLEAR(DDRD, PIND2);					// Input pin for button INT0  "SCHMITT_TRIGGER On/Off"
	BIT_CLEAR(DDRD, PIND3);					// Input pin for button INT1  "Global On/Off"
	BIT_CLEAR(DDRA, PINA0);					// Input pin for button
	BIT_CLEAR(DDRA, PINA1);					// Input pin for button
	
	BIT_SET(DDRD, PIND5);					// Output pin status light 1
	BIT_SET(DDRD, PIND1);					// Output pin status light 2
	BIT_SET(DDRD, PIND6);					// Output pin status light 3
	
	
	BIT_SET(PORTD, PIND2);					// Pull up res for input pin
	BIT_SET(PORTD, PIND3);					// Pull up res for input pin
	BIT_SET(PORTA, PINA0);					// Pull up res for input pin
	BIT_SET(PORTA, PINA1);					// Pull up res for input pin

	
	GIMSK|=((1<<INT1)|(1<<INT0)); 							// Enable interrupt on INT0 & INT1
	MCUCR&=~((1<<ISC01)|(1<<ISC00)|(1<<ISC11)|(1<<ISC10));	//LOW LEVER INTERRUPT


	sei();									// Enable global interrupts


	set_led_count(default_led_count);
	set_sleep_mode(SLEEP_MODE_PWR_DOWN);	// Set Sleep Mode

	
	while (1)
	{
		
		// First button
		if( ! BIT_CHECK(PINA,PINA1) ) {
			if(is_blinking) {
				start_8_bit_timer(); // restart timer
				is_blinking = false;
			} else {
				if(indicators_state) {
					disable_indicators();
					start_8_bit_timer();
					indicators_state = false;	
				} else {
					enable_indicators();
					stop_8_bit_timer();
					indicators_state = true;
				}
			}
			_delay_ms(400);
		}
		
		if( ! BIT_CHECK(PINA,PINA0) ) {
			adjust_led_count();
			set_led_count(default_led_count);
			_delay_ms(400);
		}

		
		
		if(go_to_sleep) {
			set_led_count(LED_COUNT_NONE);			// Disable LEDs
			disable_indicators();
						
			cli();									// Disable Interrupts
			sleep_enable();							// Enable Sleep Mode
			//sleep_bod_disable();					// Disable the Brown Out Detector (during sleep)
			sei();									// Enable Interrupts
			sleep_cpu();							// Go to Sleep ...
			sleep_disable();						// Entrance point
			
			indicators_state ? enable_indicators() : start_8_bit_timer();
			}
	}
}

