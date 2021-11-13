#ifndef DAC_H_
#define DAC_H_

#define F_CPU 1000000
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <avr/eeprom.h>
#include <avr/pgmspace.h>

int sinewave;
const int sine_table[200];

void SPI_init(void);
void custom_delay(int);
void sinewave_out(void);



#endif /* DAC_H_ */