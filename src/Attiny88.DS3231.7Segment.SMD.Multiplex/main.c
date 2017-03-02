/*
 * Attiny88.DS3231.7Segment.SMD.Multiplex main
 *
 * Created: 28/02/2017 12:53:32
 * Author : rmikhaylenko
 */ 
#define F_CPU 1000000

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <avr/power.h>
#include <avr/sleep.h>
#include <stdbool.h>

#include "i2cmaster.h"
#include "binary.h"

#define BIT_SET(a,b) ((a) |= (1<<(b)))
#define BIT_CLE(a,b) ((a) &= ~(1<<(b)))
#define BIT_FLIP(a,b) ((a) ^= (1<<(b)))
#define BIT_CHECK(a,b) ((a) & (1<<(b)))

#define FIRST_ON	((PORTD) |= (1<<(PIND7)))
#define FIRST_OFF	((PORTD) &= ~(1<<(PIND7)))
#define SECOND_ON	((PORTD) |= (1<<(PIND6)))
#define SECOND_OFF	((PORTD) &= ~(1<<(PIND6)))
#define THIRD_ON	((PORTD) |= (1<<(PIND5)))
#define THIRD_OFF	((PORTD) &= ~(1<<(PIND5)))
#define FOUR_ON		((PORTA) |= (1<<(PINA3)))
#define FOUR_OFF	((PORTA) &= ~(1<<(PINA3)))

#define DIGIT_ONE 			0x7B
#define DIGIT_TWO			0x4C
#define DIGIT_THREE			0x68
#define DIGIT_FOUR			0x39
#define DIGIT_FIVE			0xA8
#define DIGIT_SIX			0x88
#define DIGIT_SEVEN			0x7A
#define DIGIT_EIGHT			0x08
#define DIGIT_NINE			0x28
#define DIGIT_ZERO			0x0A
#define DIGIT_ALL_DISABLE	0xFF
#define DIGIT_ALL_ENABLE	0x00






/************************************************************************/
/*	START OF CLOCK FUNCTIONS                                            */
/************************************************************************/
volatile uint8_t minutes_register	= 0;
volatile uint8_t hours_register		= 0;

volatile uint8_t digit_dec_minutes	= 0;
volatile uint8_t digit_minutes		= 0;
volatile uint8_t digit_dec_hours	= 0;
volatile uint8_t digit_hours		= 0;

uint8_t dec_do_bcd(uint8_t val) {
	return ( (val/10*16) + (val%10) );
}

uint8_t bcd_to_dec(uint8_t val) {
	return ( (val/16*10) + (val%16) );
}

uint8_t convert(uint8_t digit) {
	switch (digit) {
		case 1: return DIGIT_ONE; 	break;
		case 2: return DIGIT_TWO; 	break;
		case 3: return DIGIT_THREE; break;
		case 4: return DIGIT_FOUR;  break;
		case 5: return DIGIT_FIVE;  break;
		case 6: return DIGIT_SIX;   break;
		case 7: return DIGIT_SEVEN; break;
		case 8: return DIGIT_EIGHT; break;
		case 9: return DIGIT_NINE;  break;
		case 0: return DIGIT_ZERO;  break;
		default: return DIGIT_ALL_DISABLE;
	}
}

void set_12h_format(void) {
	i2c_start_wait(0xD0);					// set device address and WRITE mode
	i2c_write(0x02); 						// write "write to" byte
	i2c_write(0x40);  						// set 12H format
	i2c_stop();
}

void set_seconds(uint8_t second) {
	i2c_start_wait(0xD0);					// set device address and WRITE mode
	i2c_write(0x00); 						// write "write to" byte
	i2c_write(dec_do_bcd(second));  		// set minute converting from decimal to BCD
	i2c_stop();
}

void set_minutes(uint8_t minute) {
	i2c_start_wait(0xD0);					// set device address and WRITE mode
	i2c_write(0x01); 						// write "write to" byte
	i2c_write(dec_do_bcd(minute));  		// set minute converting from decimal to BCD
	i2c_stop();
}

void set_hours(uint8_t hours) {
	i2c_start_wait(0xD0);						// set device address and WRITE mode
	i2c_write(0x02); 							// write "write to" byte
	i2c_write(dec_do_bcd(hours) | 0b01000000);	// set hours without loosing 12H format
	i2c_stop();
}

void get_clock(void) {
	i2c_start_wait(0xD0);					// set device address and write mode
	i2c_write(0x01); 						// write "read from" byte
	i2c_rep_start(0xD1); 					// set device address and read mode
	
	minutes_register	= i2c_readAck(); 	// Read one byte
	hours_register		= i2c_readNak(); 	// Read second byte and send END
	i2c_stop();
	
	digit_dec_minutes	= convert((minutes_register >> 4) & B0111);
	digit_minutes		= convert(minutes_register & B00001111);
	digit_dec_hours		= convert((hours_register >> 4) & B1);
	digit_hours			= convert(hours_register & B00001111);
	BIT_CLE(digit_hours, PINB3); 				// enable dot segment
}

/*	Button processing */
void adjust_clock(void) {
	get_clock();
	
	uint8_t hour	= bcd_to_dec(hours_register & 0b00011111);
	uint8_t minute	= bcd_to_dec(minutes_register);
	
	if(minute>=59) {
		minute=0;
		if(hour >= 12) {
			hour = 1;
			} else {
			++hour;
		}
		set_hours(hour);
		} else {
		++minute;
	}
	set_seconds(0); // let zero seconds as well
	set_minutes(minute);
	get_clock();
}

/************************************************************************/
/*	END OF CLOCK FUNCTIONS                                              */
/************************************************************************/





volatile uint8_t timer_interrupt = 0;
/************************************************************************/
/*	Getting clock data every 26 seconds									*/
/*	Implemented on a 8 bit timer overflow								*/
/************************************************************************/
ISR(TIMER0_OVF_vect) {
	timer_interrupt++;
	if(timer_interrupt > 100) {
		timer_interrupt=0;
		get_clock();
	}
}


/************************************************************************/
/*	Button processing by INT1 interrupt (PD3 pin low state)				*/
/*	This will turn on LEFT LED LIGHT									*/           
/************************************************************************/
ISR(INT1_vect) {
	BIT_FLIP(PORTC, PINC1);
	_delay_ms(200);
}

volatile bool sleep = false;
/************************************************************************/
/*	Button processing by INT1 interrupt (PD2 pin low state)				*/
/*	This will turn on RIGHT LED LIGHT and enable DAILY SLEEP MODE		*/
/************************************************************************/
ISR(INT0_vect) {
	sleep = (sleep == true) ? false : true;
	
	if(sleep) {
		BIT_SET(PORTC, PC0);
	} else {
		BIT_CLE(PORTC, PC0);
	}
	//BIT_FLIP(PORTC, PINC0);
	_delay_ms(200);
}



int main(void)
{
	BIT_SET(ACSR, ACD);							// Disable Analog Comparator
	BIT_CLE(ADCSRB, ADEN);						// Disable Analog to Digital Converter

	i2c_init();									// Initialize I2C interface
	set_12h_format(); 							// This will also clear hour register


	DDRB	= 0xFF; 							// Set all pins of PORTB as output
	PORTB	= 0x00;								// Ground all segments (TURN ON).
	
	DDRD = (1<<PIND5) | (1<<PIND6) | (1<<PIND7);// DIGITS ANODE (set as output):	PD5=>3 PD6=>2 PD7=>1
	DDRA = (1<<PINA3);							// DIGITS ANODE (set as output):	PA3=>4
	
	BIT_SET(DDRC, PINC0);							// Set LEFT LED PIN as output
	BIT_SET(DDRC, PINC1);							// Set RIGHT LED PIN as output
	
	//BIT_CLE(DDRD, PD6);							// Input pin for button
	//BIT_SET(PORTD, PD6);						// Pull up res for input pin

	// 8 Bit timer. Overflow routine  - ISR(TIMER0_OVF_vect)
	TIMSK0	= 1<<TOIE0;				 	// Enable overflow interrupt by timer T0
	TCCR0A	= (1<<CS00) | (1<<CS02); 	// Set up timer at F_CPU/1024 prescaler
	TCNT0	= 0x00; 		 		 	// Zero timer (start it)
	
	// ENABLE INT1 interrupt
	EICRA&=~((1<<ISC11)|(1<<ISC10)|(1<<ISC00)|(1<<ISC01));	// Set LOW LEVEL interrupt for INT0 & INT1
	EIMSK|=((1<<INT1)|(1<<INT0)); 							// Enable interrupt on INT1
	
	BIT_CLE(DDRD, PIND3);					// Input pin for button INT1  "Light On/Off"
	BIT_CLE(DDRD, PIND2);					// Input pin for button INT0  "Sleep Mode On/Off"
	BIT_CLE(DDRC, PINC2);					// Input pin for button PINC2 "Adjust Clock"
	
	BIT_SET(PORTD, PIND3);					// Pull up res for input pin INT1  "Light On/Off"
	BIT_SET(PORTD, PIND2);					// Pull up res for input pin INT0  "Sleep Mode On/Off"
	BIT_SET(PORTC, PINC2);					// Pull up res for input pin PINC2 "Adjust Clock"

	sei();									// Enable global interrupts

	//BIT_SET(PORTC, PC1);
	//BIT_SET(PORTC, PC0);
	
	while(1) {
		if(sleep) {
			FIRST_OFF;
			SECOND_OFF;
			THIRD_OFF;
			FOUR_OFF;

			cli();							// Disable Interrupts
			sleep_enable();					// Enable Sleep Mode
			set_sleep_mode(SLEEP_MODE_PWR_DOWN);	// Set Sleep Mode
			sleep_bod_disable();			// Disable the Brown Out Detector (during sleep)
			sei();							// Enable Interrupts
			sleep_cpu();					// Go to Sleep
			sleep_disable();				// Entrance point
		}		
	
		if( ! BIT_CHECK(PINC,PINC2) ) {
			adjust_clock();
			_delay_ms(200);
		}
	
		if(digit_dec_hours != DIGIT_ZERO) { // Do not show first digit if it is zero
			PORTB = digit_dec_hours;		FIRST_ON;	_delay_ms(500);	FIRST_OFF;
		}

		PORTB = digit_hours;			SECOND_ON;	_delay_ms(500);	SECOND_OFF;
		PORTB = digit_dec_minutes;		THIRD_ON;	_delay_ms(500);	THIRD_OFF;
		PORTB = digit_minutes;			FOUR_ON;	_delay_ms(500);	FOUR_OFF;
	}


}

