#ifndef SPI_H_
#define SPI_H_

#define F_CPU 1000000
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <avr/eeprom.h>
#include <avr/pgmspace.h>

void SPI_init(void);

#endif /* SPI_H_ */