/*
	DS3231 clock implemented on Attiny88 using 5611-BSR 7 segment display. Common anode.
	(c) Ruslan Mikhaylenko 27.12.2016 mirusnet@gmail.com
*/

//#define F_CPU 31250
#define F_CPU 1000000
/*	by Default: F_CPU = 8000000Mhz / 8(divider by fuses) = 1000000. 
	Should be defined before delay.h
	In this case F_CPU=31250 because of clock_prescale_set(clock_div_256); 8Mhz/256
*/

/*	TIMERS
	T[period (sec)] = (256 or 65536 [8/16 bit] * Prescaler) / F_CPU 
	Example T = (256*1024)/1000000 = 0.26
	So that the timer will be overflowed every 0.26 sec, ~ 4Hz. 
*/


#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/power.h>
#include <avr/sleep.h>
#include <avr/wdt.h>
#include <util/delay.h>
#include "i2cmaster.h"
#include "binary.h"
#include "digits.h"
#include "clock.h"


#define BIT_SET(a,b) ((a) |= (1<<(b)))
#define BIT_CLE(a,b) ((a) &= ~(1<<(b)))
#define BIT_FLIP(a,b) ((a) ^= (1<<(b)))
#define BIT_CHECK(a,b) ((a) & (1<<(b)))


/*
ISR(TIMER0_OVF_vect) {
	get_clock(); 
}
*/

void refresh(void) {
	uint16_t minutes_hours = get_clock();
		
	uint8_t hhours_register = minutes_hours & 0x00FF;
	uint8_t minutes_register = ((minutes_hours & 0xFF00)>>8);
	
	uint8_t hours_register = (hhours_register & 0b00011111);
	//uint8_t minutes_register = (minutes_hours>>8);
	
	set_digit_1((hours_register >> 4) & B1); 
	set_digit_2(hours_register & B1111); 
	set_digit_3((minutes_register >> 4) & B111); 
	set_digit_4(minutes_register & B1111);
}

ISR(WDT_vect) {
	refresh();
	WDTCSR |= (1<<WDIE);
}


/*	Button processing by INT1 interrupt (PD3 pin low state) */
ISR(INT1_vect) {
	adjust_clock();
	refresh();
}


int main(void) {
	//clock_prescale_set(clock_div_256);	// 8MHz/256 = 31250Hz
	
	i2c_init();
	//set_12h_format(); 					//This will also clear hour register
	
	BIT_SET(ACSR, ACD);					// Disable Analog Comparator
	BIT_CLE(ADCSRB, ADEN);				// Disable Analog to Digital Converter
	
	// PORT DIRECTIONS 0 INPUT 1 OUTPUT
	DDRA = B11111011;
	DDRB = B11111111;
	DDRC = B11111111;
	DDRD = B11100111;
	
	// PORT STATE 0 LOW 1 HIGH
	PORTA = B00000000;
	PORTB = B00000000;
	PORTC = B00000000;
	PORTD = B00001000; // Internal pull up res on PD3 for INT1 button

	// ENABLE INT1 interrupt
	EICRA&=~((1<<ISC11)|(1<<ISC10));	// Set LOW LEVEL interrupt
	EIMSK|=(1<<INT1); 					// Enable interrupt on INT1

	
	/*
	// 8 Bit timer. Overflow routine  - ISR(TIMER0_OVF_vect)
	TIMSK0	= 1<<TOIE0;				 // Enable overflow interrupt by timer T0
	TCCR0A	= (1<<CS00) | (1<<CS02); // Set up timer at F_CPU/1024 prescaler
	TCNT0	= 0x00; 		 		 // Zero timer (start it)
	*/

	wdt_reset();
	wdt_enable(WDTO_8S);
	WDTCSR |= (1<<WDIE);

	//set_sleep_mode(SLEEP_MODE_PWR_DOWN);	// Set Sleep Mode
	sei();									// Enable global interrupts
	
	while(1) {
		cli();					// Disable Interrupts
		sleep_enable();			// Enable Sleep Mode  
		sleep_bod_disable();	// Disable the Brown Out Detector (during sleep)
		sei();					// Enable Interrupts
		sleep_cpu();			// Go to Sleep
		sleep_disable();		// Entrance point
	}
}