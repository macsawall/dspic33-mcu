#include "SPI.h"
#include "GlobalDefinitions.h"

void SPI_init(void)
{
	setLOW(PRR0, PRSPI); //clear PRSPI bit to allow SPI
	setHIGH(DDRB, PB7); //SDO output
	setLOW(DDRB, PB6); //SDI input
	setHIGH(DDRB, PB5); //SCK output
	setHIGH(DDRB, PB4); //SS output (the /SS pin on the ATmega must be set to output to avoid problems as a Master device. So we will just use it as our slave select output for the LCD)
	setHIGH(PORTB, PB4); //active low SS disabled
	setHIGH(SPCR, MSTR); //master mode enabled
	setHIGH(SPCR, CPOL); //SCK is high when idle (LCD)
	setHIGH(SPCR, CPHA); //data sampled on leading/rising edge of clock (LCD)
	setHIGH(SPCR, SPR1); //SCK frequency = fosc/128
	setHIGH(SPCR, SPR0); //
	setHIGH(SPCR, SPE); //enable SPI
}