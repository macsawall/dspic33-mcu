#ifndef TEMPERATURE_H_
#define TEMPERATURE_H_

#define F_CPU 1000000
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <avr/eeprom.h>
#include <avr/pgmspace.h>

extern volatile unsigned int temperature;
volatile unsigned int degrees_F;

void temperature_print(volatile unsigned int temp);
void temperature_check(volatile unsigned int temp);

#endif /* TEMPERATURE_H_ */