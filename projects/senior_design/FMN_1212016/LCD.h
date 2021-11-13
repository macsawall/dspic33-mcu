#ifndef LCD_H_
#define LCD_H_

#define F_CPU 1000000
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <avr/eeprom.h>
#include <avr/pgmspace.h>

extern volatile unsigned char SPI_received_byte;

void LCD_init(void);
void LCD_command(char, char);
void LCD_string(char string[]);
void LCD_string_P(const char *string);
void LCD_char(char character);
void LCD_SPIreset(void);



#endif /* LCD_H_ */