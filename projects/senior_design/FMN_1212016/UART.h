#ifndef UART_H_
#define UART_H_

#define F_CPU 1000000
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <avr/eeprom.h>
#include <avr/pgmspace.h>

extern volatile unsigned char UART1_received_byte;
extern volatile unsigned char UART0_received_byte;
extern volatile unsigned char GPS_comma_count;
extern volatile unsigned char FONA_FLAG;

void UART1_init(void);
void UART0_init(void);
void UART1_TX(volatile unsigned char string[]);
void UART0_TX(volatile unsigned char string[]); //write (transmit) strings to RN41 Bluetooth
void UARTtoLCD(volatile unsigned char key); //echoes single characters back to putty and LCD
void send_data(volatile unsigned char rn41);

#endif /* UART_H_ */