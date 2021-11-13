
#ifndef TIMERS_H_
#define TIMERS_H_

#define F_CPU 1000000
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <avr/eeprom.h>
#include <avr/pgmspace.h>

void timer1_init(void);



#endif /* TIMERS_H_ */