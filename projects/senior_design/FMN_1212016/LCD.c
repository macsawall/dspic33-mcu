#include "LCD.h"
#include "GlobalDefinitions.h"


void LCD_SPIreset(void)
{
	setHIGH(SPCR, CPOL); //SCK is high when idle
	setHIGH(SPCR, CPHA); //data sampled on leading/rising edge of clock
}

void LCD_init(void)
{
	setHIGH(DDRB, PB4); //SS output (the /SS pin on the ATmega must be set to output to avoid problems as a Master device. So we will just use it as our slave select output for the LCD)
	setHIGH(PORTB, PB4); //active low SS disabled
	setHIGH(SPCR, CPOL); //SCK is high when idle
	setHIGH(SPCR, CPHA); //data sampled on leading/rising edge of clock
}

void LCD_command(char byte2, char byte3)
{
	LCD_SPIreset();
	setLOW(PORTB, PB4); //active low SS enabled
	SPDR = 0xFE;
	while(!checkHIGH(SPSR, SPIF)); //wait until transfer complete
	SPDR = byte2;
	while(!checkHIGH(SPSR, SPIF)); //wait until transfer complete
	
	if (byte3 != 0)
	{
		SPDR = byte3;
		while(!checkHIGH(SPSR, SPIF)); //wait until transfer complete
		setHIGH(PORTB, PB4); //active low SS disabled
	}
	
	setHIGH(PORTB, PB4); //active low SS disabled
}

void LCD_string(char string[])
{
	LCD_SPIreset();
	char garbage = 0;
	int i = 0;
	while(string[i] != 0) //loop though string until all characters written
	{
		setLOW(PORTB, PB4); //active low SS enabled
		SPDR = string[i];
		while(!checkHIGH(SPSR, SPIF)); //wait until transfer complete
		setHIGH(PORTB, PB4); //active low SS disabled
		i++;
	}
}

void LCD_string_P(const char *string)
{
	LCD_SPIreset();
	char garbage = 0;
	int i = 0;
	while(pgm_read_byte(string) != 0) //loop though string until all characters written
	{
		setLOW(PORTB, PB4); //active low SS enabled
		SPDR = pgm_read_byte(string++);
		while(!checkHIGH(SPSR, SPIF)); //wait until transfer complete
		setHIGH(PORTB, PB4); //active low SS disabled
		i++;
	}
}

void LCD_char(char character)
{
	LCD_SPIreset();
	setLOW(PORTB, PB4); //active low SS enabled
	SPDR = character;
	while(!checkHIGH(SPSR, SPIF)); //wait until transfer complete
	setHIGH(PORTB, PB4); //active low SS disabled
}