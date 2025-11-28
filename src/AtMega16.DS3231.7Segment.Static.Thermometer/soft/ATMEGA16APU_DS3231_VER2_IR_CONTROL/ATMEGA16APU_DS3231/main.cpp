/*
* ATMEGA16APU_DS3231.cpp
*
* Created: 28-Nov-2025
* Author : mirusnet@gmail.com
*/

#define F_CPU 1000000


#include <avr/io.h>
#include <util/delay.h>
#include <avr/wdt.h>
#include <avr/sleep.h>
#include <avr/power.h>
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


uint8_t get_temperature(void) {
	i2c_start_wait(0xD0);					// set device address and write mode
	i2c_write(0x11); 						// write "read from" byte
	i2c_rep_start(0xD1); 					// set device address and read mode
	uint8_t temperature = i2c_readNak(); 	// read one byte
	i2c_stop();
	return temperature;
}


void disableBlinking() {
	
	uint8_t controlByte = 0b00000100;		// INTCN=1 disable square wave output
	i2c_start_wait(0xD0);					// set device address and WRITE mode
	i2c_write(0x0E); 						// write "write to" byte
	i2c_write(controlByte);					// set controlByte
	i2c_stop();
	
}


// Get clock Every 16 seconds
ISR(TIMER1_OVF_vect)
{
	set_all_digits(get_temperature());
}


int main(void)
{
	//DISABLE JTAG
	MCUCSR=(1<<JTD);
	MCUCSR=(1<<JTD);
	_delay_ms(50);
	
	
	BIT_SET(ACSR, ACD);									// Disable Analog Comparator
	BIT_CLE(ADCSRA, ADEN);								// Disable Analog to Digital Converter
	
	/* Turn off WDT WatchDog*/
	wdt_reset();
	WDTCR |= (1<<WDTOE) | (1<<WDE);
	WDTCR = 0x00;
	
	// ENABLE INT1 and INT0 interrupts
	MCUCR&=~((1<<ISC11)|(1<<ISC10)|(1<<ISC00)|(1<<ISC01));	// Set LOW LEVEL interrupt for INT0 & INT1
	GICR|=((1<<INT1)|(1<<INT0)); 							// Enable interrupt on INT0 & INT1
	
	// Enable timer1 overflow interrupt
	TIMSK |=(1<<TOIE1);
	// Set Timer prescaler 256
	BIT_CLE(TCCR1B, CS10);
	BIT_CLE(TCCR1B, CS11);
	BIT_SET(TCCR1B, CS12);

	
	DDRA=0xFF;
	PORTA=0x00;
	
	DDRB=0xFF;
	PORTB=0x00;
	
	DDRC=0xFF;
	PORTC=0x00;
	
	DDRD=0xFF;
	PORTD=0x00;
	
	
	BIT_CLE(DDRD, PIND3);  // Top Button pin as input
	BIT_SET(PORTD, PIND3); // PULL UP RES for Top button pin
	
	BIT_CLE(DDRD, PIND2);  // Bottom Button pin as input
	BIT_SET(PORTD, PIND2); // PULL UP RES for Bottom button pin
	
	i2c_init();
	disableBlinking();
	_delay_ms(50);
	

	disable_all_digits();	
	set_all_digits(get_temperature()); // GET CLOCK FIRST TIME, UPDATE DISPLAY

	set_sleep_mode(SLEEP_MODE_IDLE);
	sei(); // Enable global interrupt
	
		
	while (1)
	{	
		cli();							// Disable Interrupts
		sleep_enable();					// Enable Sleep Mode
		sei();							// Enable Interrupts
		sleep_cpu();					// Go to Sleep
		sleep_disable();				// Entrance point
	}

}

