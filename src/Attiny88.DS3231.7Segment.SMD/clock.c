#include "i2cmaster.h"
#include "binary.h"

/*
For using i2cmaster library the i2c_address must be:
(i2c_address<<1)|0x01	READ
(i2c_address<<1)		WRITE
Because i2c uses 7 bit for address and 1 bit for direction 
X-X-X-X-X-X-X-D
*/

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

void get_clock(volatile uint8_t *minutes_register, volatile uint8_t *hours_register) {
	i2c_start_wait(0xD0);					// set device address and write mode
	i2c_write(0x01); 					// write "read from" byte
	i2c_rep_start(0xD1); 					// set device address and read mode
	
	*minutes_register	= i2c_readAck(); 	// Read one byte
	*hours_register		= i2c_readNak(); 	// Read second byte and send END
	i2c_stop();
	
/*
	set_digit_1((hours_register >> 4) & B1); 
	set_digit_2(hours_register & B1111); 
	set_digit_3((minutes_register >> 4) & B111); 
	set_digit_4(minutes_register & B1111);
*/
}




void adjust_clock(uint8_t minutes_register, uint8_t hours_register) {
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
}


