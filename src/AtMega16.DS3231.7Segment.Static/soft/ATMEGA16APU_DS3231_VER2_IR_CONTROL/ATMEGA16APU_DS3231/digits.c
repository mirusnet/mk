#include <avr/io.h>
#include "digits.h"



#define BIT_SET(a,b) ((a) |= (1<<(b)))
#define BIT_CLE(a,b) ((a) &= ~(1<<(b)))
#define BIT_FLIP(a,b) ((a) ^= (1<<(b)))
#define BIT_CHECK(a,b) ((a) & (1<<(b)))

// FIRST DIGIT
#define S1A_PIN PIND6
#define S1B_PIN PIND5
#define S1C_PIN PINC3
#define S1D_PIN PINC4
#define S1E_PIN PINC5
#define S1F_PIN PINC7
#define S1G_PIN PINC6

#define S1A_PORT PORTD
#define S1B_PORT PORTD
#define S1C_PORT PORTC
#define S1D_PORT PORTC
#define S1E_PORT PORTC
#define S1F_PORT PORTC
#define S1G_PORT PORTC


// SECOND DIGIT
#define S2A_PIN PINB7
#define S2B_PIN PINB6

#define S2C_PIN PINC2 //
#define S2D_PIN PIND7 //
#define S2E_PIN PIND4
#define S2F_PIN PIND0
#define S2G_PIN PIND1

#define S2A_PORT PORTB
#define S2B_PORT PORTB
#define S2C_PORT PORTC //
#define S2D_PORT PORTD //
#define S2E_PORT PORTD
#define S2F_PORT PORTD
#define S2G_PORT PORTD


// THIRD DIGIT
#define S3A_PIN PINB0
#define S3B_PIN PINA7
#define S3C_PIN PINB3
#define S3D_PIN PINB5
#define S3E_PIN PINB4
#define S3F_PIN PINB1
#define S3G_PIN PINB2

#define S3A_PORT PORTB
#define S3B_PORT PORTA
#define S3C_PORT PORTB
#define S3D_PORT PORTB
#define S3E_PORT PORTB
#define S3F_PORT PORTB
#define S3G_PORT PORTB


// FOUR DIGIT
#define S4A_PIN PINA1
#define S4B_PIN PINA0
#define S4C_PIN PINA4
#define S4D_PIN PINA6
#define S4E_PIN PINA5
#define S4F_PIN PINA2
#define S4G_PIN PINA3
#define S4A_PORT PORTA
#define S4B_PORT PORTA
#define S4C_PORT PORTA
#define S4D_PORT PORTA
#define S4E_PORT PORTA
#define S4F_PORT PORTA
#define S4G_PORT PORTA

void set_digit_4(uint8_t digit) {
switch (digit) {
        case 0: 
			BIT_CLE(S4A_PORT, S4A_PIN);
			BIT_CLE(S4B_PORT, S4B_PIN);
			BIT_CLE(S4C_PORT, S4C_PIN);
			BIT_CLE(S4D_PORT, S4D_PIN);
			BIT_CLE(S4E_PORT, S4E_PIN);
			BIT_CLE(S4F_PORT, S4F_PIN);
			BIT_SET(S4G_PORT, S4G_PIN);
		break;
        case 1:
			BIT_SET(S4A_PORT, S4A_PIN); 
			BIT_CLE(S4B_PORT, S4B_PIN);
			BIT_CLE(S4C_PORT, S4C_PIN);
			BIT_SET(S4D_PORT, S4D_PIN); 
			BIT_SET(S4E_PORT, S4E_PIN); 
			BIT_SET(S4F_PORT, S4F_PIN); 
			BIT_SET(S4G_PORT, S4G_PIN);
		break;
		case 2:
			BIT_CLE(S4A_PORT, S4A_PIN); 
			BIT_CLE(S4B_PORT, S4B_PIN);
			BIT_SET(S4C_PORT, S4C_PIN);
			BIT_CLE(S4D_PORT, S4D_PIN); 
			BIT_CLE(S4E_PORT, S4E_PIN); 
			BIT_SET(S4F_PORT, S4F_PIN); 
			BIT_CLE(S4G_PORT, S4G_PIN);
			break;
		case 3:
			BIT_CLE(S4A_PORT, S4A_PIN); 
			BIT_CLE(S4B_PORT, S4B_PIN);
			BIT_CLE(S4C_PORT, S4C_PIN);
			BIT_CLE(S4D_PORT, S4D_PIN); 
			BIT_SET(S4E_PORT, S4E_PIN); 
			BIT_SET(S4F_PORT, S4F_PIN); 
			BIT_CLE(S4G_PORT, S4G_PIN);
		break;
		case 4:
			BIT_SET(S4A_PORT, S4A_PIN); 
			BIT_CLE(S4B_PORT, S4B_PIN);
			BIT_CLE(S4C_PORT, S4C_PIN);
			BIT_SET(S4D_PORT, S4D_PIN); 
			BIT_SET(S4E_PORT, S4E_PIN); 
			BIT_CLE(S4F_PORT, S4F_PIN); 
			BIT_CLE(S4G_PORT, S4G_PIN);
		break;
		case 5:
			BIT_CLE(S4A_PORT, S4A_PIN); 
			BIT_SET(S4B_PORT, S4B_PIN);
			BIT_CLE(S4C_PORT, S4C_PIN);
			BIT_CLE(S4D_PORT, S4D_PIN); 
			BIT_SET(S4E_PORT, S4E_PIN); 
			BIT_CLE(S4F_PORT, S4F_PIN); 
			BIT_CLE(S4G_PORT, S4G_PIN);
		break;
		case 6:
			BIT_CLE(S4A_PORT, S4A_PIN); 
			BIT_SET(S4B_PORT, S4B_PIN);
			BIT_CLE(S4C_PORT, S4C_PIN);
			BIT_CLE(S4D_PORT, S4D_PIN); 
			BIT_CLE(S4E_PORT, S4E_PIN); 
			BIT_CLE(S4F_PORT, S4F_PIN); 
			BIT_CLE(S4G_PORT, S4G_PIN);
		break;
		case 7:
			BIT_CLE(S4A_PORT, S4A_PIN); 
			BIT_CLE(S4B_PORT, S4B_PIN);
			BIT_CLE(S4C_PORT, S4C_PIN);
			BIT_SET(S4D_PORT, S4D_PIN); 
			BIT_SET(S4E_PORT, S4E_PIN); 
			BIT_SET(S4F_PORT, S4F_PIN); 
			BIT_SET(S4G_PORT, S4G_PIN);
		break;
		case 8:
			BIT_CLE(S4A_PORT, S4A_PIN); 
			BIT_CLE(S4B_PORT, S4B_PIN);
			BIT_CLE(S4C_PORT, S4C_PIN);
			BIT_CLE(S4D_PORT, S4D_PIN); 
			BIT_CLE(S4E_PORT, S4E_PIN); 
			BIT_CLE(S4F_PORT, S4F_PIN); 
			BIT_CLE(S4G_PORT, S4G_PIN);
		break;
		case 9:
			BIT_CLE(S4A_PORT, S4A_PIN); 
			BIT_CLE(S4B_PORT, S4B_PIN);
			BIT_CLE(S4C_PORT, S4C_PIN);
			BIT_CLE(S4D_PORT, S4D_PIN); 
			BIT_SET(S4E_PORT, S4E_PIN); 
			BIT_CLE(S4F_PORT, S4F_PIN); 
			BIT_CLE(S4G_PORT, S4G_PIN);
		break;
		default: // print _
			BIT_SET(S4A_PORT, S4A_PIN);
			BIT_SET(S4B_PORT, S4B_PIN);
			BIT_SET(S4C_PORT, S4C_PIN);
			BIT_CLE(S4D_PORT, S4D_PIN);
			BIT_SET(S4E_PORT, S4E_PIN);
			BIT_SET(S4F_PORT, S4F_PIN);
			BIT_SET(S4G_PORT, S4G_PIN); 
    }
}

void set_digit_3(uint8_t digit) {
switch (digit) {
        case 0: 
			BIT_CLE(S3A_PORT, S3A_PIN);
			BIT_CLE(S3B_PORT, S3B_PIN);
			BIT_CLE(S3C_PORT, S3C_PIN);
			BIT_CLE(S3D_PORT, S3D_PIN);
			BIT_CLE(S3E_PORT, S3E_PIN);
			BIT_CLE(S3F_PORT, S3F_PIN);
			BIT_SET(S3G_PORT, S3G_PIN);
		break;
        case 1:
			BIT_SET(S3A_PORT, S3A_PIN); 
			BIT_CLE(S3B_PORT, S3B_PIN);
			BIT_CLE(S3C_PORT, S3C_PIN);
			BIT_SET(S3D_PORT, S3D_PIN); 
			BIT_SET(S3E_PORT, S3E_PIN); 
			BIT_SET(S3F_PORT, S3F_PIN); 
			BIT_SET(S3G_PORT, S3G_PIN);
		break;
		case 2:
			BIT_CLE(S3A_PORT, S3A_PIN); 
			BIT_CLE(S3B_PORT, S3B_PIN);
			BIT_SET(S3C_PORT, S3C_PIN);
			BIT_CLE(S3D_PORT, S3D_PIN); 
			BIT_CLE(S3E_PORT, S3E_PIN); 
			BIT_SET(S3F_PORT, S3F_PIN); 
			BIT_CLE(S3G_PORT, S3G_PIN);
		break;
		case 3:
			BIT_CLE(S3A_PORT, S3A_PIN); 
			BIT_CLE(S3B_PORT, S3B_PIN);
			BIT_CLE(S3C_PORT, S3C_PIN);
			BIT_CLE(S3D_PORT, S3D_PIN); 
			BIT_SET(S3E_PORT, S3E_PIN); 
			BIT_SET(S3F_PORT, S3F_PIN); 
			BIT_CLE(S3G_PORT, S3G_PIN);
		break;
		case 4:
			BIT_SET(S3A_PORT, S3A_PIN); 
			BIT_CLE(S3B_PORT, S3B_PIN);
			BIT_CLE(S3C_PORT, S3C_PIN);
			BIT_SET(S3D_PORT, S3D_PIN); 
			BIT_SET(S3E_PORT, S3E_PIN); 
			BIT_CLE(S3F_PORT, S3F_PIN); 
			BIT_CLE(S3G_PORT, S3G_PIN);
		break;
		case 5:
			BIT_CLE(S3A_PORT, S3A_PIN); 
			BIT_SET(S3B_PORT, S3B_PIN);
			BIT_CLE(S3C_PORT, S3C_PIN);
			BIT_CLE(S3D_PORT, S3D_PIN); 
			BIT_SET(S3E_PORT, S3E_PIN); 
			BIT_CLE(S3F_PORT, S3F_PIN); 
			BIT_CLE(S3G_PORT, S3G_PIN);
		break;
		case 6:
			BIT_CLE(S3A_PORT, S3A_PIN); 
			BIT_SET(S3B_PORT, S3B_PIN);
			BIT_CLE(S3C_PORT, S3C_PIN);
			BIT_CLE(S3D_PORT, S3D_PIN); 
			BIT_CLE(S3E_PORT, S3E_PIN); 
			BIT_CLE(S3F_PORT, S3F_PIN); 
			BIT_CLE(S3G_PORT, S3G_PIN);
		break;
		case 7:
			BIT_CLE(S3A_PORT, S3A_PIN); 
			BIT_CLE(S3B_PORT, S3B_PIN);
			BIT_CLE(S3C_PORT, S3C_PIN);
			BIT_SET(S3D_PORT, S3D_PIN); 
			BIT_SET(S3E_PORT, S3E_PIN); 
			BIT_SET(S3F_PORT, S3F_PIN); 
			BIT_SET(S3G_PORT, S3G_PIN);
		break;
		case 8:
			BIT_CLE(S3A_PORT, S3A_PIN); 
			BIT_CLE(S3B_PORT, S3B_PIN);
			BIT_CLE(S3C_PORT, S3C_PIN);
			BIT_CLE(S3D_PORT, S3D_PIN); 
			BIT_CLE(S3E_PORT, S3E_PIN); 
			BIT_CLE(S3F_PORT, S3F_PIN); 
			BIT_CLE(S3G_PORT, S3G_PIN);
		break;
		case 9:
			BIT_CLE(S3A_PORT, S3A_PIN); 
			BIT_CLE(S3B_PORT, S3B_PIN);
			BIT_CLE(S3C_PORT, S3C_PIN);
			BIT_CLE(S3D_PORT, S3D_PIN); 
			BIT_SET(S3E_PORT, S3E_PIN); 
			BIT_CLE(S3F_PORT, S3F_PIN); 
			BIT_CLE(S3G_PORT, S3G_PIN);
		break;
		default: // print _
			BIT_SET(S3A_PORT, S3A_PIN);
			BIT_SET(S3B_PORT, S3B_PIN);
			BIT_SET(S3C_PORT, S3C_PIN);
			BIT_CLE(S3D_PORT, S3D_PIN);
			BIT_SET(S3E_PORT, S3E_PIN);
			BIT_SET(S3F_PORT, S3F_PIN);
			BIT_SET(S3G_PORT, S3G_PIN); 
    }
}

void set_digit_2(uint8_t digit) {
switch (digit) {
        case 0: 
			BIT_CLE(S2A_PORT, S2A_PIN);
			BIT_CLE(S2B_PORT, S2B_PIN);
			BIT_CLE(S2C_PORT, S2C_PIN);
			BIT_CLE(S2D_PORT, S2D_PIN);
			BIT_CLE(S2E_PORT, S2E_PIN);
			BIT_CLE(S2F_PORT, S2F_PIN);
			BIT_SET(S2G_PORT, S2G_PIN);
		break;
        case 1:
			BIT_SET(S2A_PORT, S2A_PIN); 
			BIT_CLE(S2B_PORT, S2B_PIN);
			BIT_CLE(S2C_PORT, S2C_PIN);
			BIT_SET(S2D_PORT, S2D_PIN); 
			BIT_SET(S2E_PORT, S2E_PIN); 
			BIT_SET(S2F_PORT, S2F_PIN); 
			BIT_SET(S2G_PORT, S2G_PIN);
		break;
		case 2:
			BIT_CLE(S2A_PORT, S2A_PIN); 
			BIT_CLE(S2B_PORT, S2B_PIN);
			BIT_SET(S2C_PORT, S2C_PIN);
			BIT_CLE(S2D_PORT, S2D_PIN); 
			BIT_CLE(S2E_PORT, S2E_PIN); 
			BIT_SET(S2F_PORT, S2F_PIN); 
			BIT_CLE(S2G_PORT, S2G_PIN);
		break;
		case 3:
			BIT_CLE(S2A_PORT, S2A_PIN); 
			BIT_CLE(S2B_PORT, S2B_PIN);
			BIT_CLE(S2C_PORT, S2C_PIN);
			BIT_CLE(S2D_PORT, S2D_PIN); 
			BIT_SET(S2E_PORT, S2E_PIN); 
			BIT_SET(S2F_PORT, S2F_PIN); 
			BIT_CLE(S2G_PORT, S2G_PIN);
		break;
		case 4:
			BIT_SET(S2A_PORT, S2A_PIN); 
			BIT_CLE(S2B_PORT, S2B_PIN);
			BIT_CLE(S2C_PORT, S2C_PIN);
			BIT_SET(S2D_PORT, S2D_PIN); 
			BIT_SET(S2E_PORT, S2E_PIN); 
			BIT_CLE(S2F_PORT, S2F_PIN); 
			BIT_CLE(S2G_PORT, S2G_PIN);
		break;
		case 5:
			BIT_CLE(S2A_PORT, S2A_PIN); 
			BIT_SET(S2B_PORT, S2B_PIN);
			BIT_CLE(S2C_PORT, S2C_PIN);
			BIT_CLE(S2D_PORT, S2D_PIN); 
			BIT_SET(S2E_PORT, S2E_PIN); 
			BIT_CLE(S2F_PORT, S2F_PIN); 
			BIT_CLE(S2G_PORT, S2G_PIN);
		break;
		case 6:
			BIT_CLE(S2A_PORT, S2A_PIN); 
			BIT_SET(S2B_PORT, S2B_PIN);
			BIT_CLE(S2C_PORT, S2C_PIN);
			BIT_CLE(S2D_PORT, S2D_PIN); 
			BIT_CLE(S2E_PORT, S2E_PIN); 
			BIT_CLE(S2F_PORT, S2F_PIN); 
			BIT_CLE(S2G_PORT, S2G_PIN);
		break;
		case 7:
			BIT_CLE(S2A_PORT, S2A_PIN); 
			BIT_CLE(S2B_PORT, S2B_PIN);
			BIT_CLE(S2C_PORT, S2C_PIN);
			BIT_SET(S2D_PORT, S2D_PIN); 
			BIT_SET(S2E_PORT, S2E_PIN); 
			BIT_SET(S2F_PORT, S2F_PIN); 
			BIT_SET(S2G_PORT, S2G_PIN);
		break;
		case 8:
			BIT_CLE(S2A_PORT, S2A_PIN); 
			BIT_CLE(S2B_PORT, S2B_PIN);
			BIT_CLE(S2C_PORT, S2C_PIN);
			BIT_CLE(S2D_PORT, S2D_PIN); 
			BIT_CLE(S2E_PORT, S2E_PIN); 
			BIT_CLE(S2F_PORT, S2F_PIN); 
			BIT_CLE(S2G_PORT, S2G_PIN);
		break;
		case 9:
			BIT_CLE(S2A_PORT, S2A_PIN); 
			BIT_CLE(S2B_PORT, S2B_PIN);
			BIT_CLE(S2C_PORT, S2C_PIN);
			BIT_CLE(S2D_PORT, S2D_PIN); 
			BIT_SET(S2E_PORT, S2E_PIN); 
			BIT_CLE(S2F_PORT, S2F_PIN); 
			BIT_CLE(S2G_PORT, S2G_PIN);
		break;
		default: // print _
			BIT_SET(S2A_PORT, S2A_PIN);
			BIT_SET(S2B_PORT, S2B_PIN);
			BIT_SET(S2C_PORT, S2C_PIN);
			BIT_CLE(S2D_PORT, S2D_PIN);
			BIT_SET(S2E_PORT, S2E_PIN);
			BIT_SET(S2F_PORT, S2F_PIN);
			BIT_SET(S2G_PORT, S2G_PIN);
    }
}

void set_digit_1(uint8_t digit) {
switch (digit) {
	case 0:
	BIT_SET(S1A_PORT, S1A_PIN); // DO NOT ENABLE THE DIGIT AT ALL
	BIT_SET(S1B_PORT, S1B_PIN);
	BIT_SET(S1C_PORT, S1C_PIN);
	BIT_SET(S1D_PORT, S1D_PIN);
	BIT_SET(S1E_PORT, S1E_PIN);
	BIT_SET(S1F_PORT, S1F_PIN);
	BIT_SET(S1G_PORT, S1G_PIN);
	break;
	 case 1:
	 BIT_SET(S1A_PORT, S1A_PIN);
	 BIT_CLE(S1B_PORT, S1B_PIN);
	 BIT_CLE(S1C_PORT, S1C_PIN);
	 BIT_SET(S1D_PORT, S1D_PIN);
	 BIT_SET(S1E_PORT, S1E_PIN);
	 BIT_SET(S1F_PORT, S1F_PIN);
	 BIT_SET(S1G_PORT, S1G_PIN);
	 break;
	 case 2:
	 BIT_CLE(S1A_PORT, S1A_PIN);
	 BIT_CLE(S1B_PORT, S1B_PIN);
	 BIT_SET(S1C_PORT, S1C_PIN);
	 BIT_CLE(S1D_PORT, S1D_PIN);
	 BIT_CLE(S1E_PORT, S1E_PIN);
	 BIT_SET(S1F_PORT, S1F_PIN);
	 BIT_CLE(S1G_PORT, S1G_PIN);
	 break;
	 case 3:
	 BIT_CLE(S1A_PORT, S1A_PIN);
	 BIT_CLE(S1B_PORT, S1B_PIN);
	 BIT_CLE(S1C_PORT, S1C_PIN);
	 BIT_CLE(S1D_PORT, S1D_PIN);
	 BIT_SET(S1E_PORT, S1E_PIN);
	 BIT_SET(S1F_PORT, S1F_PIN);
	 BIT_CLE(S1G_PORT, S1G_PIN);
	 break;
	 case 4:
	 BIT_SET(S1A_PORT, S1A_PIN);
	 BIT_CLE(S1B_PORT, S1B_PIN);
	 BIT_CLE(S1C_PORT, S1C_PIN);
	 BIT_SET(S1D_PORT, S1D_PIN);
	 BIT_SET(S1E_PORT, S1E_PIN);
	 BIT_CLE(S1F_PORT, S1F_PIN);
	 BIT_CLE(S1G_PORT, S1G_PIN);
	 break;
	 case 5:
	 BIT_CLE(S1A_PORT, S1A_PIN);
	 BIT_SET(S1B_PORT, S1B_PIN);
	 BIT_CLE(S1C_PORT, S1C_PIN);
	 BIT_CLE(S1D_PORT, S1D_PIN);
	 BIT_SET(S1E_PORT, S1E_PIN);
	 BIT_CLE(S1F_PORT, S1F_PIN);
	 BIT_CLE(S1G_PORT, S1G_PIN);
	 break;
	 case 6:
	 BIT_CLE(S1A_PORT, S1A_PIN);
	 BIT_SET(S1B_PORT, S1B_PIN);
	 BIT_CLE(S1C_PORT, S1C_PIN);
	 BIT_CLE(S1D_PORT, S1D_PIN);
	 BIT_CLE(S1E_PORT, S1E_PIN);
	 BIT_CLE(S1F_PORT, S1F_PIN);
	 BIT_CLE(S1G_PORT, S1G_PIN);
	 break;
	 case 7:
	 BIT_CLE(S1A_PORT, S1A_PIN);
	 BIT_CLE(S1B_PORT, S1B_PIN);
	 BIT_CLE(S1C_PORT, S1C_PIN);
	 BIT_SET(S1D_PORT, S1D_PIN);
	 BIT_SET(S1E_PORT, S1E_PIN);
	 BIT_SET(S1F_PORT, S1F_PIN);
	 BIT_SET(S1G_PORT, S1G_PIN);
	 break;
	 case 8:
	 BIT_CLE(S1A_PORT, S1A_PIN);
	 BIT_CLE(S1B_PORT, S1B_PIN);
	 BIT_CLE(S1C_PORT, S1C_PIN);
	 BIT_CLE(S1D_PORT, S1D_PIN);
	 BIT_CLE(S1E_PORT, S1E_PIN);
	 BIT_CLE(S1F_PORT, S1F_PIN);
	 BIT_CLE(S1G_PORT, S1G_PIN);
	 break;
	 case 9:
	 BIT_CLE(S1A_PORT, S1A_PIN);
	 BIT_CLE(S1B_PORT, S1B_PIN);
	 BIT_CLE(S1C_PORT, S1C_PIN);
	 BIT_CLE(S1D_PORT, S1D_PIN);
	 BIT_SET(S1E_PORT, S1E_PIN);
	 BIT_CLE(S1F_PORT, S1F_PIN);
	 BIT_CLE(S1G_PORT, S1G_PIN);
	 break;
	 default: // print _
	 BIT_SET(S1A_PORT, S1A_PIN);
	 BIT_SET(S1B_PORT, S1B_PIN);
	 BIT_SET(S1C_PORT, S1C_PIN);
	 BIT_CLE(S1D_PORT, S1D_PIN);
	 BIT_SET(S1E_PORT, S1E_PIN);
	 BIT_SET(S1F_PORT, S1F_PIN);
	 BIT_SET(S1G_PORT, S1G_PIN);
}
}

/*
	set_all_digits parses two bytes of data returned by ds3231 from minutes and hours registers 
	and set digits accordingly
*/

void set_all_digits(uint16_t mmhh) {
	set_digit_1((mmhh&0x30)>>4); 
	set_digit_2(mmhh&0xF); 
	set_digit_3((mmhh>>12)&0x7); 
	set_digit_4((mmhh>>8)&0xF);
}

void disable_all_digits() {
	 
	// DIGIT 1
	BIT_SET(S1A_PORT, S1A_PIN);
	BIT_SET(S1B_PORT, S1B_PIN);
	BIT_SET(S1C_PORT, S1C_PIN);
	BIT_SET(S1D_PORT, S1D_PIN);
	BIT_SET(S1E_PORT, S1E_PIN);
	BIT_SET(S1F_PORT, S1F_PIN);
	BIT_SET(S1G_PORT, S1G_PIN);
	 
	//DIGIT 2
	BIT_SET(S2A_PORT, S2A_PIN);
	BIT_SET(S2B_PORT, S2B_PIN);
	BIT_SET(S2C_PORT, S2C_PIN);
	BIT_SET(S2D_PORT, S2D_PIN);
	BIT_SET(S2E_PORT, S2E_PIN);
	BIT_SET(S2F_PORT, S2F_PIN);
	BIT_SET(S2G_PORT, S2G_PIN);
		 
	//DIGIT 3
	BIT_SET(S3A_PORT, S3A_PIN);
	BIT_SET(S3B_PORT, S3B_PIN);
	BIT_SET(S3C_PORT, S3C_PIN);
	BIT_SET(S3D_PORT, S3D_PIN);
	BIT_SET(S3E_PORT, S3E_PIN);
	BIT_SET(S3F_PORT, S3F_PIN);
	BIT_SET(S3G_PORT, S3G_PIN);
			 
	//DIGIT 4
	BIT_SET(S4A_PORT, S4A_PIN);
	BIT_SET(S4B_PORT, S4B_PIN);
	BIT_SET(S4C_PORT, S4C_PIN);
	BIT_SET(S4D_PORT, S4D_PIN);
	BIT_SET(S4E_PORT, S4E_PIN);
	BIT_SET(S4F_PORT, S4F_PIN);
	BIT_SET(S4G_PORT, S4G_PIN);
}
