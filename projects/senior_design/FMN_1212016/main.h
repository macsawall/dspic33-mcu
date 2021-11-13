#ifndef INCFILE1_H_
#define INCFILE1_H_

#define F_CPU 1000000
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <avr/eeprom.h>
#include <avr/pgmspace.h>

//extern volatile unsigned char SPI_received_byte;
//extern volatile unsigned char UART0_received_byte;


int main(void);
void main_processes(void);


#endif /* INCFILE1_H_ */ 