#ifndef KEYPAD_H_
#define KEYPAD_H_

#define F_CPU 1000000
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <avr/eeprom.h>
#include <avr/pgmspace.h>

extern volatile unsigned char SPI_received_byte;

void keypad_SPIreset(void);
void keypad_init(void);
void keypad_read(void);



#endif /* KEYPAD_H_ */