/*
	DS3231 clock implemented on Attiny88 using 5611-BSR 7 segment display. Common anode.
	(c) Ruslan Mikhaylenko 27.12.2016 mirusnet@gmail.com
*/

#define F_CPU 31250
//#define F_CPU 1000000
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
#include <util/delay.h>
#include "i2cmaster.h"
#include "binary.h"
#include "digits.h"

/*
For using i2cmaster library the i2c_address must be:
(i2c_address<<1)|0x01	READ
(i2c_address<<1)		WRITE
Because i2c uses 7 bit for address and 1 bit for direction 
X-X-X-X-X-X-X-D
*/

#define BIT_SET(a,b) ((a) |= (1<<(b)))
#define BIT_CLE(a,b) ((a) &= ~(1<<(b)))
#define BIT_FLIP(a,b) ((a) ^= (1<<(b)))
#define BIT_CHECK(a,b) ((a) & (1<<(b)))

volatile uint8_t minutes_register	= 0;
volatile uint8_t hours_register		= 0;

uint8_t dec_do_bcd(uint8_t val) {
	return ( (val/10*16) + (val%10) );
}

uint8_t bcd_to_dec(uint8_t val) {
	return ( (val/16*10) + (val%16) );
}


void set_12h_format(void) {
	i2c_start_wait(0xD0);					// set device address and WRITE mode
	i2c_write(0x02); 						// write "write to" byte
	i2c_write(0x40);  						// set 12H format
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
	
	set_digit_1((hours_register >> 4) & B1); 
	set_digit_2(hours_register & B1111); 
	set_digit_3((minutes_register >> 4) & B111); 
	set_digit_4(minutes_register & B1111);
}


ISR(TIMER0_OVF_vect) {
		/*	We can read the clock directly in the interrupt
			as it is being executed every 8 sec
			and we dont have any other interrupt in place
		*/
		get_clock(); 
}


ISR(INT1_vect) {
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
	set_minutes(minute);
	get_clock();
}


int main(void) {
	clock_prescale_set(clock_div_256);	// 8MHz/256 = 31250Hz
	i2c_init();
	set_12h_format(); 					//This will also clear hour register
	
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

	// 8 Bit timer. Overflow routine  - ISR(TIMER0_OVF_vect)
	TIMSK0	= 1<<TOIE0;				 // Enable overflow interrupt by timer T0
	TCCR0A	= (1<<CS00) | (1<<CS02); // Set up timer at F_CPU/1024 prescaler
	TCNT0	= 0x00; 		 		 // Zero timer (start it)
	sei(); 				 			 // Enable global interrupts

	
	while(1) {
	}
}