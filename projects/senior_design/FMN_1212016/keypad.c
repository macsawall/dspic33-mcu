#include "GlobalDefinitions.h"
#include "keypad.h"
#include "SPI.h"


volatile unsigned char SPI_received_byte = 'G';

void keypad_init(void)
{
	setHIGH(EICRA, ISC21); //detect interrupt on a falling edge
	setLOW(EICRA, ISC20); //
	setHIGH(EIMSK, INT2); //enable external interrupts on INT2
	setLOW(DDRB, PB2); //set REVERSE SS *input for keyboard
	setHIGH(PORTB, PB2); //set keyboard SS input high to activate pull-up resistor
}

void keypad_SPIreset(void)
{
	setLOW(SPCR, CPOL); //keypad needs these settings for SPI because ATtiny is weird
	setLOW(SPCR, CPHA);
}

void keypad_read(void)
{
	while(checkHIGH(PINB, PB2)); //wait until reverse SS goes low. Will go low once a key is pressed on keypad
	keypad_SPIreset();
	SPDR = SPI_received_byte; //initiate transfer?
	while(!checkHIGH(SPSR, SPIF)); //wait until transfer complete
	SPI_received_byte = SPDR;
}