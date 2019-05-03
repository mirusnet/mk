/*
 * Attiny88.DS3231.7Segment.SMD.Multiplex main
 *
 * Created: 28/02/2018 12:29:32
 * Author : Ruslan Mikhaylenko mirusnet@gmail.com
 */ 
#define F_CPU 1000000

#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/power.h>
#include <avr/sleep.h>
#include <avr/wdt.h>
#include <util/delay.h>
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

#define DIGIT_S				0xA8
#define DIGIT_L				0x8F
#define DIGIT_E				0x8C
#define DIGIT_O				0x0A
#define DIGIT_N				0x1A
#define DIGIT_F				0x9C


// We are using custom delay with count * 10ms cycles, so that 20*10=200ms
#define DEFAULT_PRESS_DELAY  20

/************************************************************************/
/*	START OF CLOCK FUNCTIONS                                            */
/************************************************************************/

volatile uint8_t hour			= 0;
volatile uint8_t minute			= 0;


volatile uint8_t digit_dec_minutes	= 0;
volatile uint8_t digit_minutes		= 0;
volatile uint8_t digit_dec_hours	= 0;
volatile uint8_t digit_hours		= 0;



volatile uint8_t press_delay		= DEFAULT_PRESS_DELAY;

volatile bool clock_mode = false;

static inline void custom_delay_ten_ms(uint8_t count) {
	while(count--) {
		_delay_ms(10);
	}
}

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

void set_24h_format(void) {
	i2c_start_wait(0xD0);					// set device address and WRITE mode
	i2c_write(0x02); 						// write "write to" byte
	i2c_write(0x00);  						// set 24H format
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
	i2c_start_wait(0xD0);					// set device address and WRITE mode
	i2c_write(0x02); 						// write "write to" byte
	i2c_write(dec_do_bcd(hours));			// set hours 
	i2c_stop();
}



void get_temperature() {
	i2c_start_wait(0xD0);					// set device address and write mode
	i2c_write(0x11); 						// write "read from" byte
	i2c_rep_start(0xD1); 					// set device address and read mode
	
	uint8_t temperature			= i2c_readAck(); 	// Read one byte
	uint8_t temperature_decimal	= (i2c_readNak() >> 6); 	// Read another byte and send END
	i2c_stop();
	
	switch (temperature_decimal) {
		case 0:		digit_minutes = DIGIT_ZERO;			break;
		case 1:		digit_minutes = DIGIT_THREE; 		break;
		case 2:		digit_minutes = DIGIT_FIVE; 		break;
		case 3:		digit_minutes = DIGIT_EIGHT;		break;
		default:	digit_minutes = DIGIT_ALL_DISABLE;
	}
	
	
	if(temperature>>7 == 1) {
		digit_dec_hours	= 253;		   // Minus sign to be shown
	} else {
		digit_dec_hours = DIGIT_ZERO;  // This will not be shown by display at all
	}
	
	
	digit_dec_minutes	= convert((temperature& 0b01111111) % 10);				
	digit_hours			= convert((temperature& 0b01111111) / 10);						
	
	BIT_CLE(digit_dec_minutes, PINB3); 									// enable dot segment
}


void get_clock(void) {
	i2c_start_wait(0xD0);					// set device address and write mode
	i2c_write(0x01); 						// write "read from" byte
	i2c_rep_start(0xD1); 					// set device address and read mode
	
	uint8_t minutes_register	= i2c_readAck(); 	// Read one byte
	uint8_t hours_register		= i2c_readNak(); 	// Read second byte and send END
	i2c_stop();
	
	hour	= bcd_to_dec(hours_register & 0b00111111);
	minute	= bcd_to_dec(minutes_register);
	
	digit_dec_minutes	= convert((minutes_register >> 4) & B0111);
	digit_minutes		= convert(minutes_register & B00001111);
	digit_dec_hours		= convert((hours_register >> 4) & B11);
	digit_hours			= convert(hours_register & B00001111);
	BIT_CLE(digit_hours, PINB3); 				// enable dot segment
}


void adjust_clock(void) {
	get_clock();

	if(minute>=59) {
		minute=0;
		if(hour >= 23) {
			hour = 0;
			} else {
			++hour;
		}
		set_hours(hour);
		} else {
		++minute;
	}
	set_seconds(0);	// let zero seconds as well
	set_minutes(minute);
	get_clock();
}



/************************************************************************/
/* The function adjusts clock's time by adding minute                   */
/************************************************************************/
void check_and_adjust_clock() {
	if( ! BIT_CHECK(PINC,PINC2) ) {
		if(press_delay >1) {
			--press_delay;
		}
		adjust_clock();
		custom_delay_ten_ms(press_delay);
	}
}



/************************************************************************/
/*	END OF CLOCK FUNCTIONS                                              */
/************************************************************************/

/************************************************************************/
/*	START OF DISPLAY FUNCTIONS                                          */
/************************************************************************/



void display_disable() {
	FIRST_OFF; 	SECOND_OFF; THIRD_OFF; FOUR_OFF;
}


void display_values() {
	if(digit_dec_hours != DIGIT_ZERO) { // Do not show first digit if it is zero
		PORTB = digit_dec_hours;		FIRST_ON;	_delay_ms(5);	FIRST_OFF;
	}
	PORTB = digit_hours;			SECOND_ON;	_delay_ms(5);	SECOND_OFF;
	PORTB = digit_dec_minutes;		THIRD_ON;	_delay_ms(5);	THIRD_OFF;
	PORTB = digit_minutes;			FOUR_ON;	_delay_ms(5);	FOUR_OFF;
}

/************************************************************************/
/*	END OF DISPLAY FUNCTIONS                                            */
/************************************************************************/




/************************************************************************/
/*	START OF INTERRUPT ROUTINES	                                        */
/************************************************************************/

/************************************************************************/
/*	WATCHDOG timer fires every 8 seconds, we get values only on each    */
/*  third interrupt, so that each 24 seconds                            */
/************************************************************************/


ISR(WDT_vect) {
	if(clock_mode) {
		get_clock();
	} else {
        get_temperature();	
	}
	
	WDTCSR |= (1<<WDIE);	// Set watch dog action to fire interrupt instead of reset
}



/************************************************************************/
/*	Getting clock data every 26 seconds									*/
/*	Implemented on a 8 bit timer overflow								*/
/************************************************************************/
/*
volatile uint8_t timer_interrupt = 0;
ISR(TIMER0_OVF_vect) {
	timer_interrupt++;
	if(timer_interrupt > 100) {
		timer_interrupt=0;
		get_clock();
	}
}
*/

/************************************************************************/
/*	Getting clock data every 67 seconds									*/
/*	Implemented on a 16 bit timer overflow								*/
/************************************************************************/
/*
ISR(TIMER1_OVF_vect) {
	clock_mode ? get_clock() : get_temperature();
}
*/

/************************************************************************/
/*	Button processing by INT1 interrupt (PD3 pin low state)				*/
/*	This will turn on LEFT LED FLASH LIGHT								*/           
/************************************************************************/
ISR(INT1_vect) {
	display_disable();		// JUST CLRSCR
	BIT_FLIP(PORTC, PINC1);
	_delay_ms(200);
}

/************************************************************************/
/*	Button processing by INT1 interrupt (PD2 pin low state)				*/
/*	This will turn on BLUE LED LIGHT and enable DAILY SLEEP MODE		*/
/************************************************************************/
ISR(INT0_vect) {
	display_disable();		// JUST CLRSCR
	if(clock_mode) {
		get_temperature();
		clock_mode = false;
	} else {
		get_clock();
		clock_mode = true;
	}
	//BIT_FLIP(PORTC, PINC0); // Turn off sleep indicator immediatelly
	_delay_ms(400);
}
/************************************************************************/
/*	END OF INTERRUPT ROUTINES	                                        */
/************************************************************************/




int main(void)
{
	//clock_prescale_set(clock_div_256);
	
	BIT_SET(ACSR, ACD);									// Disable Analog Comparator
	BIT_CLE(ADCSRB, ADEN);	     						// Disable Analog to Digital Converter

	i2c_init();											// Initialize I2C interface
	set_24h_format(); 									// This will also clear hour register
	
	set_hours(9);										// Initial clock settings
	set_minutes(30);									// Initial clock settings
	
	//_delay_ms(2000);
	
	//get_clock();
		
	DDRB	= 0xFF; 									// Set all pins of PORTB as output
	PORTB	= 0x00;										// Ground all segments (TURN ON).
	
	DDRD = (1<<PIND5) | (1<<PIND6) | (1<<PIND7);		// DIGITS ANODE (set as output):	PD5=>3 PD6=>2 PD7=>1
	DDRA = (1<<PINA3);									// DIGITS ANODE (set as output):	PA3=>4
	
	BIT_SET(DDRC, PINC0);								// Set LEFT LED PIN as output
	BIT_SET(DDRC, PINC1);								// Set RIGHT LED PIN as output

	/* // 8 Bit timer. Overflow routine  - ISR(TIMER0_OVF_vect)
	TIMSK0	= 1<<TOIE0;				 					// Enable overflow interrupt by timer T0
	TCCR0A	= (1<<CS00) | (1<<CS02); 					// Set up timer at F_CPU/1024 prescaler
	TCNT0	= 0x00; 		 		 					// Zero timer (start it)
	*/

	/* 16 bit timer.  Overflow routine  - ISR(TIMER1_OVF_vect) */
	/*
	TIMSK1	= 1<<TOIE1;				 // Enable overflow interrupt by timer T1
	TCCR1B	= (1<<CS10) | (1<<CS12); // Set up timer at F_CPU/1024
	TCNT1	= 0x00; 		 		 // Zero timer (start it)
	*/


	
	wdt_reset();										// Reset watch dog timer
	wdt_enable(WDTO_8S);								// Set watch dog to run every 8 seconds
	WDTCSR |= (1<<WDIE);								// Set watch dog action to fire interrupt instead of reset
	
	
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

	get_temperature();
	
	while(1) {
		if(clock_mode) {
			check_and_adjust_clock();
		
			if(BIT_CHECK(PINC,PINC2)) {			// Adjust clock button was released
				press_delay	= DEFAULT_PRESS_DELAY;
			} 
		}
		display_values();			
	}
}