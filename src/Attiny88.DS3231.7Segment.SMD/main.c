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
#include <util/delay.h>
#include "i2cmaster.h"
#include "binary.h"


/*
For using i2cmaster library the i2c_address must be:
(i2c_address<<1)|0x01	READ
(i2c_address<<1)		WRITE
Because i2c uses 7 bit for address and 1 bit for direction 
X-X-X-X-X-X-X-D
*/

#define BIT_SET(a,b) ((a) |= (1<<(b)))
#define BIT_CLEAR(a,b) ((a) &= ~(1<<(b)))
#define BIT_FLIP(a,b) ((a) ^= (1<<(b)))
#define BIT_CHECK(a,b) ((a) & (1<<(b)))


#define FIRST_ON	((PORTA) |= (1<<(PA1)))
#define FIRST_OFF	((PORTA) &= ~(1<<(PA1)))


volatile uint8_t minutes_register	= 0;
volatile uint8_t hours_register		= 0;

volatile uint8_t dec_minutes	= 0;
volatile uint8_t minutes		= 0;
volatile uint8_t dec_hours	= 0;
volatile uint8_t hours		= 0;



/*
uint8_t convert(uint8_t digit) {
	switch (digit) {
        case 1: return DIGIT_ONE; 	break;
        case 0: return DIGIT_ZERO;  break;
	    default: return DIGIT_ALL_DISABLE;
    }
}
*/

//volatile uint8_t interrupt = 0;


void get_clock(void) {
	i2c_start_wait(0xD0);					// set device address and write mode
	i2c_write(0x01); 						// write "read from" byte
	i2c_rep_start(0xD1); 					// set device address and read mode
	
	minutes_register	= i2c_readAck(); 	// Read one byte
	hours_register		= i2c_readNak(); 	// Read second byte and send END
	i2c_stop();
	
	dec_minutes		= (minutes_register >> 4) & B111;
	minutes			= minutes_register & B1111;
	dec_hours		= (hours_register >> 4) & B1;
	hours			= hours_register & B1111;
}


//ISR(TIMER0_OVF_vect) {
		/*	We can read the clock directly in the interrupt
			as it is being executed every 26 sec
			and we dont have any other interrupt in place
		*/
//		get_clock(); 
//}






int main(void) {
	//clock_prescale_set(clock_div_16); //8MHz/16 = 500KHz
	i2c_init();
	
	BIT_SET(ACSR,ACD);					// Disable Analog Comparator
	BIT_CLEAR(ADCSRB, ADEN);			// Disable Analog to Digital Converter
	
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
	
	
	
	/*
	//8 Bit timer. Overflow routine  - ISR(TIMER0_OVF_vect)
	TIMSK	= 1<<TOIE0;				 // Enable overflow interrupt by timer T0
	TCCR0B	= (1<<CS00) | (1<<CS02); // Set up timer at F_CPU/1024 prescaler
	TCNT0	= 0x00; 		 		 // Zero timer (start it)
	sei(); 				 			 // Enable global interrupts
*/
	
	
	
	
	while(1) {
		get_clock();
		if(minutes != 0) {
			for(int i=0; i<minutes; i++) {
				PORTC = B11111111;
				_delay_ms(500);
				PORTC = B00000000;
				_delay_ms(500);
			}
		}
		
		_delay_ms(3000);
	
	}
}