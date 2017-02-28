#ifndef CLOCK_H
#define CLOCK_H  

uint8_t dec_do_bcd(uint8_t val);
uint8_t bcd_to_dec(uint8_t val);

void set_12h_format(void);
void set_minutes(uint8_t minute);
void set_hours(uint8_t hours);

uint16_t get_clock(void);
void adjust_clock(void);
#endif
