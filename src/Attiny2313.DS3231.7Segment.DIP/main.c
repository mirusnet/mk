/*
	DS3231 clock implemented on Attiny2313A using 5641-BSR 7 segment display
	(c) Ruslan Mikhaylenko 28.11.2016 mirusnet@gmail.com
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
#define SECOND_ON	((PORTA) |= (1<<(PA0)))
#define SECOND_OFF	((PORTA) &= ~(1<<(PA0)))
#define THIRD_ON	((PORTD) |= (1<<(PD2)))
#define THIRD_OFF	((PORTD) &= ~(1<<(PD2)))
#define FOUR_ON		((PORTD) |= (1<<(PD1)))
#define FOUR_OFF	((PORTD) &= ~(1<<(PD1)))



#define DIGIT_ONE 			0xBE
#define DIGIT_TWO			0x62
#define DIGIT_THREE			0x2A
#define DIGIT_FOUR			0x3C
#define DIGIT_FIVE			0x29
#define DIGIT_SIX			0x21
#define DIGIT_SEVEN			0xBA
#define DIGIT_EIGHT			0x20
#define DIGIT_NINE			0x28
#define DIGIT_ZERO			0xA0
#define DIGIT_ALL_DISABLE	0xFF
#define DIGIT_ALL_ENABLE	0x00

uint8_t dec_do_bcd(uint8_t val) {
	return ( (val/10*16) + (val%10) );
}

uint8_t bcd_to_dec(uint8_t val) {
	return ( (val/16*10) + (val%16) );
}


volatile uint8_t minutes_register	= 0;
volatile uint8_t hours_register		= 0;

volatile uint8_t digit_dec_minutes	= 0;
volatile uint8_t digit_minutes		= 0;
volatile uint8_t digit_dec_hours	= 0;
volatile uint8_t digit_hours		= 0;



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


volatile uint8_t interrupt = 0;

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
	BIT_CLEAR(digit_hours, PB5); 			// enable dot segment
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


ISR(TIMER0_OVF_vect) {
	interrupt++;
	if(interrupt > 100) {
		interrupt=0;
		/*	We can read the clock directly in the interrupt
			as it is being executed every 26 sec
			and we dont have any other interrupt in place
		*/
		get_clock(); 
	}
}






int main(void) {
	//clock_prescale_set(clock_div_256); //8MHz/16 = 500KHz
	i2c_init();
	
	ACSR|=(1<<ACD);					//Disable Analog Comparator
 
	DDRB = 0xFF; 					//Set all pins of PORTB as output
	DDRD = (1<<PD2) | (1<<PD1);		//DIGITS ANODE:	PD2=>3 PD1=>4
	BIT_CLEAR(DDRD, PD6);			//Input pin for button 
	DDRA = (1<<PA0) | (1<<PA1);		//DIGITS ANODE:	PA0=>2 PA1=>1
	PORTB = 0x00;					//Ground all segments (TURN ON).
	BIT_SET(PORTD, PD6);			//Pull up res for input pin 
	
	//8 Bit timer. Overflow routine  - ISR(TIMER0_OVF_vect)
	TIMSK	= 1<<TOIE0;				 // Enable overflow interrupt by timer T0
	TCCR0B	= (1<<CS00) | (1<<CS02); // Set up timer at F_CPU/1024 prescaler
	TCNT0	= 0x00; 		 		 // Zero timer (start it)
	sei(); 				 			 // Enable global interrupts

	
	
	while(1) {
		if( ! BIT_CHECK(PIND,PD6) ) {
			adjust_clock();
			_delay_ms(200);
		}	
	
		if(digit_dec_hours != DIGIT_ZERO) { // Do not show first digit if it is zero
			PORTB = digit_dec_hours;		FIRST_ON;	_delay_ms(5);	FIRST_OFF;
		}
			PORTB = digit_hours;			SECOND_ON;	_delay_ms(5);	SECOND_OFF;
			PORTB = digit_dec_minutes;		THIRD_ON;	_delay_ms(5);	THIRD_OFF;
			PORTB = digit_minutes;			FOUR_ON;	_delay_ms(5);	FOUR_OFF;
	}
}