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

void set_seconds(uint8_t second) {
	i2c_start_wait(0xD0);					// set device address and WRITE mode
	i2c_write(0x00); 						// write "write to" byte
	i2c_write(dec_do_bcd(second));  		// set seconds converting from decimal to BCD
	i2c_stop(); 
}

void set_minutes(uint8_t minute) {
	i2c_start_wait(0xD0);					// set device address and WRITE mode
	i2c_write(0x01); 						// write "write to" byte
	i2c_write(dec_do_bcd(minute));  		// set minutes converting from decimal to BCD
	i2c_stop(); 
}

void set_hours(uint8_t hours) {
	i2c_start_wait(0xD0);						// set device address and WRITE mode
	i2c_write(0x02); 							// write "write to" byte
	i2c_write(dec_do_bcd(hours) | 0b01000000);	// set hours without loosing 12H format
	i2c_stop(); 
}


/*
	get_clock returns two bytes of data, minutes and hours (mmhh)
	mmhh format: 0 0 0 0 DM DM DM M M M M DH H H H H
	DM Decimal Minutes, M Minutes, DH Decimal Hours, H Hours
*/
uint16_t get_clock(void) {
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

void adjust_clock(void) {
	uint16_t minutes_hours = get_clock();
	uint8_t hour	= bcd_to_dec(minutes_hours & 0b00011111);
	uint8_t minute	= bcd_to_dec(minutes_hours>>8);
	
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
	set_seconds(0); //zero seconds as well
	set_minutes(minute);
}


