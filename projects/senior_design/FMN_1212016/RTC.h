
#ifndef RTC_H_
#define RTC_H_

#define F_CPU 1000000
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <avr/eeprom.h>
#include <avr/pgmspace.h>

volatile unsigned char RTC_raw_timer;

void RTC_split_timer(volatile unsigned char raw_time);
void RTC_print_timer(volatile unsigned char binary);
void RTC_write(volatile unsigned char reg_address, volatile unsigned char data);
volatile unsigned int RTC_read(volatile unsigned char reg_address);

#endif /* RTC_H_ */