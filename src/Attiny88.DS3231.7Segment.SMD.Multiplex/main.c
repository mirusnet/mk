/*
 * Attiny88.DS3231.7Segment.SMD.Multiplex main
 *
 * Created: 28/02/2017 12:53:32
 * Author : rmikhaylenko
 */ 
#define F_CPU 1000000

#include <avr/io.h>
#include <avr/interrupt.h>


#define BIT_SET(a,b) ((a) |= (1<<(b)))
#define BIT_CLE(a,b) ((a) &= ~(1<<(b)))
#define BIT_FLIP(a,b) ((a) ^= (1<<(b)))
#define BIT_CHECK(a,b) ((a) & (1<<(b)))

#define FIRST_ON	((PORTD) |= (1<<(PD7)))
#define FIRST_OFF	((PORTD) &= ~(1<<(PD7)))
#define SECOND_ON	((PORTD) |= (1<<(PD6)))
#define SECOND_OFF	((PORTD) &= ~(1<<(PD6)))
#define THIRD_ON	((PORTD) |= (1<<(PD5)))
#define THIRD_OFF	((PORTD) &= ~(1<<(PD5)))
#define FOUR_ON		((PORTA) |= (1<<(PA3)))
#define FOUR_OFF	((PORTA) &= ~(1<<(PA3)))

#define DIGIT_ONE 			0x7B
#define DIGIT_TWO			0x4C
#define DIGIT_THREE			0x68
#define DIGIT_FOUR			0x39
#define DIGIT_FIVE			0xA8
#define DIGIT_SIX			0x88
#define DIGIT_SEVEN			0x7A
#define DIGIT_EIGHT			0x80
#define DIGIT_NINE			0x28
#define DIGIT_ZERO			0xA0
#define DIGIT_ALL_DISABLE	0xFF
#define DIGIT_ALL_ENABLE	0x00


volatile uint8_t digit = 0;
volatile uint8_t interrupt = 0;


ISR(TIMER0_OVF_vect) {
	interrupt++;
	if(interrupt > 10) {
		interrupt=0;
		digit++;
		if(digit > 9) {
			digit = 0;
		}
	}
}


int main(void)
{
	BIT_SET(ACSR, ACD);							// Disable Analog Comparator
	BIT_CLE(ADCSRB, ADEN);						// Disable Analog to Digital Converter

	DDRB	= 0xFF; 							// Set all pins of PORTB as output
	PORTB	= 0x00;								// Ground all segments (TURN ON).
	
	DDRD = (1<<PD5) | (1<<PD6) | (1<<PD7);		// DIGITS ANODE:	PD5=>3 PD6=>2 PD7=>1
	DDRA = (1<<PA3);							// DIGITS ANODE:	PA3=>4
	
	BIT_CLE(DDRD, PD6);							// Input pin for button
	BIT_SET(PORTD, PD6);						// Pull up res for input pin

	// 8 Bit timer. Overflow routine  - ISR(TIMER0_OVF_vect)
	TIMSK0	= 1<<TOIE0;				 	// Enable overflow interrupt by timer T0
	TCCR0A	= (1<<CS00) | (1<<CS02); 	// Set up timer at F_CPU/1024 prescaler
	TCNT0	= 0x00; 		 		 	// Zero timer (start it)
	

    /* Replace with your application code */
    while (1) 
    {
    }
}

