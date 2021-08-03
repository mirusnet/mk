/*
* ATMEGA16APU_DS3231.cpp
*
* Created: 12-Jan-2021
* Author : mirusnet@gmail.com
*/

#define F_CPU 1000000

#include <avr/io.h>
#include <util/delay.h>
#include <avr/wdt.h>
#include <avr/sleep.h>
#include <avr/interrupt.h>


extern "C"
{
	#include "i2cmaster.h"
	#include "digits.h"
}




#define BIT_SET(a,b) ((a) |= (1<<(b)))
#define BIT_CLE(a,b) ((a) &= ~(1<<(b)))
#define BIT_FLIP(a,b) ((a) ^= (1<<(b)))
#define BIT_CHECK(a,b) ((a) & (1<<(b)))



uint8_t dec_do_bcd(uint8_t val) {
	return ( (val/10*16) + (val%10) );
}

uint8_t bcd_to_dec(uint8_t val) {
	return ( (val/16*10) + (val%16) );
}


// set hours format is incorrect because it also clears the time register
// we should update only sixth bit of the register
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


/*
void get_clock(void) {
	i2c_start_wait(0xD0);					// set device address and write mode
	i2c_write(0x01); 						// write "read from" byte
	i2c_rep_start(0xD1); 					// set device address and read mode
	
	uint8_t minutes_register	= i2c_readAck(); 	// Read one byte
	uint8_t hours_register		= i2c_readNak(); 	// Read second byte and send END
	i2c_stop();
	
	hour	= bcd_to_dec(hours_register & 0b00111111);
	minute	= bcd_to_dec(minutes_register);
	
}
*/

/*
	get_clock returns two bytes of data, minutes and hours (mmhh)
	mmhh format: [0 DM DM DM M M M M]   [0 12/24 DH DH H H H H]
	DM Decimal Minutes, M Minutes, DH Decimal Hours, H Hours
*/
uint16_t get_clock_hex(void) {
	uint16_t minutes_hours = 0;
	i2c_start_wait(0xD0);					// set device address and write mode
	i2c_write(0x01); 						// write "read from" byte
	i2c_rep_start(0xD1); 					// set device address and read mode
	minutes_hours = i2c_readAck();		 	// read one byte (minutes)
	minutes_hours = (minutes_hours<<8);		// shift one byte left
	minutes_hours|= i2c_readNak();			// read second byte (hours) and send END
	i2c_stop();
	return minutes_hours;
}

uint8_t getControlByte() {
	i2c_start_wait(0xD0);					// set device address and write mode
	i2c_write(0x0E); 						// write "read from" byte
	i2c_rep_start(0xD1); 					// set device address and read mode
	
	uint8_t controlByte	= i2c_readAck(); 	// Read one byte
	
	i2c_stop();
	return controlByte;
}

void enableBlinking() {
	
	uint8_t controlByte = 0b00000000;		// by this bit configuration we have 1Hz blinking enabled only when VCC is > Vpf
	i2c_start_wait(0xD0);					// set device address and WRITE mode
	i2c_write(0x0E); 						// write "write to" byte
	i2c_write(controlByte);					// set controlByte
	i2c_stop();
	
}


void delayTimes(uint8_t times) {
	for(int i=0; i<times; i++) {
		_delay_ms(10);
	}
}

/*
void buzzerShort() {
	BIT_SET(PORTC, PINC2);
	_delay_ms(30);
	BIT_CLE(PORTC, PINC2);
}


void buzzerTenTimes() {
	for(int i=0; i<10; i++) {
		buzzerShort();
		_delay_ms(100);
	}
}
*/

void adjust_clock(void) {
	uint8_t initialDelay=20;
	
	while((!BIT_CHECK(PIND, PIND7))) {
	//mmhh format: [0 DM DM DM M M M M]   [0 12/24 DH DH H H H H]
	uint16_t mmhh = get_clock_hex();
	
	uint8_t hour	= bcd_to_dec(mmhh & 0b00111111);
	uint8_t minute	= bcd_to_dec((mmhh>>8) & 0b01111111);
	
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
	
	set_seconds(0);	
	set_minutes(minute);
	
	set_all_digits(get_clock_hex());
	
		
	delayTimes(initialDelay);
	
	if(initialDelay >0) {
		--initialDelay;
	} 
	
	
	}
}



int main(void)
{
	//DISABLE JTAG
	MCUCSR=(1<<JTD);
	MCUCSR=(1<<JTD);
	_delay_ms(50);
	
	
	BIT_SET(ACSR, ACD);									// Disable Analog Comparator
	BIT_CLE(ADCSRA, ADEN);								// Disable Analog to Digital Converter
	
	
	
	DDRA=0xFF;
	PORTA=0x00;
	
	DDRB=0xFF;
	PORTB=0x00;
	
	DDRC=0xFF;
	PORTC=0x00;
	
	DDRD=0xFF;
	PORTD=0x00;
	
	
	BIT_CLE(PORTC, PINC2); // Buzzer disable
	
	BIT_CLE(DDRD, PIND7);  // Button pin as input
	BIT_SET(PORTD, PIND7); // PULL UP RES for button pin
	
	
	i2c_init();
	enableBlinking();
	_delay_ms(50);
	
	
	/*
	set_24h_format();
	*/
	
	
	/*
	set_hours(14);
	set_minutes(0);
	set_seconds(0);
	*/
	

	set_all_digits(get_clock_hex()); // GET CLOCK FIRST TIME, UPDATE DISPLAY

	uint8_t getClockDelay = 30;
	
	while (1)
	{	
		if(!BIT_CHECK(PIND, PIND7)) {
			adjust_clock();
		}
		
		// GET CLOCK EVERY 30th time, having wait 100ms => i.e. every 3000ms=3sec.
		if(getClockDelay-- <=1) {
			set_all_digits(get_clock_hex());
			getClockDelay = 30;
		}
		
		_delay_ms(100);
	}

}

