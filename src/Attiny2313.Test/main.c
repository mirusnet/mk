#define F_CPU 31250 // by clock_div_256
//F_CPU = 8000000Mhz / 8(divider by fuses) = 1000000. Should be defined before delay.h

#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <avr/power.h>

/* ATtiny 2313

*/


#define BIT_SET(a,b) ((a) |= (1<<(b)))
#define BIT_CLEAR(a,b) ((a) &= ~(1<<(b)))
#define BIT_FLIP(a,b) ((a) ^= (1<<(b)))
#define BIT_CHECK(a,b) ((a) & (1<<(b)))


#define PORTB_BIT_SET(bit)   ((PORTB) |= (1<<(bit)))
#define PORTB_BIT_CLEAR(bit) ((PORTB) &= ~(1<<bit))

/*	TIMERS
	T[period] = (256 or 65536 [8/16 bit] * Prescaler) / F_CPU 
	Example T = (256*1024)/1000000 = 0.26
	So that the timer will be overflowed every 0.26 sec, ~ 4Hz. 
*/

/*
i2c_init();
uint8_t read_one_byte(uint8_t byte_address) {
	i2c_start_wait(0xD0);			// set device address and write mode
	i2c_write(byte_address); 		// write "read from" byte
	i2c_rep_start(0xD1); 			// set device address and read mode
	uint8_t byte  = i2c_readNak(); 	// Read one byte
	i2c_stop();
	return byte;
}

	minutes_register 	= read_one_byte(0x01);
	hours_register		= read_one_byte(0x02);
*/



volatile uint8_t overflow = 0;

ISR(TIMER0_OVF_vect) {
	BIT_FLIP(PORTB, PB0);
}


int main(void)
{
	clock_prescale_set(clock_div_256); //8Mhz/128 = 31250

	
	//8 Bit timer. Overflow routine  - ISR(TIMER0_OVF_vect)
	
	TIMSK	= 1<<TOIE0;				 // Enable overflow interrupt by timer T0
	TCCR0B	= (1<<CS00) | (1<<CS02); 		 // Set up timer at F_CPU/1024
	TCNT0	= 0x00; 		 		 // Zero timer (start it)
	
	/* 16 bit timer NOT TESTED Overflow routine  - ISR(TIMER1_OVF_vect)
	TIMSK	= 1<<TOIE1;				 // Enable overflow interrupt by timer T1
	TCCR1B	= (1<<CS10) | (1<<CS12); // Set up timer at F_CPU/1024
	TCNT1	= 0x00; 		 		 // Zero timer (start it)
	*/
	
	sei(); 				 			 // Enable global interrupts
	
	
	
	//DDRB=(1<<PB0)|(1<<PB1)|(1<<PB2)|(1<<PB3);
	
	DDRB=(1<<PB0);
	
        while (1)
        {
		//	BIT_FLIP(PORTB, PB0);
		//	_delay_ms(1000);
	/*
			if(overflow > 10) {
				BIT_FLIP(PORTB, PB0);
				overflow = 0;
			}
			*/
		}
}
