
#ifndef I2C_H_
#define I2C_H_

#define F_CPU 1000000
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <avr/eeprom.h>
#include <avr/pgmspace.h>

void I2C_init(void);

#endif /* I2C_H_ */