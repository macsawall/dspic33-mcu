#include "GlobalDefinitions.h"
#include "temperature.h"
#include "LCD.h"

volatile unsigned int temperature = 0;
volatile unsigned int degrees_F = 0;

void temperature_print(volatile unsigned int temp)
{
	volatile double celsius;
	volatile int fahrenheit;
	unsigned int negative = 0;
	unsigned char digit2;
	unsigned char digit1;
	unsigned char digit0;

	celsius = ((165.0*((double)temp))/385.0) - 40.0; //celsius values range from -40 to 125. Instead of dividing by 1023, which the digital value will never reach, we divide by 385
	fahrenheit = (int)(((9.0*celsius)/5.0) + 32.0); //fahrenheit values range from -40 to 257
	
	if(fahrenheit < 100 && fahrenheit > -100) //if fahrenheit is only 2 digits
	{
		if(fahrenheit > 0) //if positive
		{
			digit2 = 0;
			digit1 = fahrenheit/10;
			digit0 = fahrenheit%10;
		}
		else //if negative, reverse 2's complement
		{
			negative = 1;
			fahrenheit = (~fahrenheit) + 1;
			digit2 = 0;
			digit1 = fahrenheit/10;
			digit0 = fahrenheit%10;
		}
	}
	else if(fahrenheit >= 100 || fahrenheit <= -100) //if fahrenheit is 3 digits
	{
		if(fahrenheit > 0) //if positive
		{
			digit2 = fahrenheit/100;
			digit1 = (fahrenheit/10)%10;
			digit0 = fahrenheit%10;
		}
		else //if negative, reverse 2's complement
		{
			negative = 1;
			fahrenheit = (~fahrenheit) + 1;
			digit2 = fahrenheit/100;
			digit1 = (fahrenheit/10)%10;
			digit0 = fahrenheit%10;
		}
	}
	
	if(negative == 1) //if negative print a minus sign first, else don't print it
	LCD_char('-');
	if(fahrenheit > 99 || fahrenheit < -99) //if there is a digit in the hundred's place print it, else don't print it
	LCD_char(digit2 + '0');
	if(fahrenheit > 9 || fahrenheit < -9) //if there is a digit in the ten's place print it, else don't print it
	LCD_char(digit1 + '0');
	LCD_char(digit0 + '0'); //always print digit in one's place
	asm volatile("Nop");
}

void temperature_check(volatile unsigned int temp)
{
	volatile double celsius;
	volatile int fahrenheit;

	celsius = ((165.0*((double)temp))/385.0) - 40.0; //celsius values range from -40 to 125. Instead of dividing by 1023, which the digital value will never reach, we divide by 385
	fahrenheit = (int)(((9.0*celsius)/5.0) + 32.0); //fahrenheit values range from -40 to 257
	
	if (fahrenheit >= 78)
		degrees_F = 1;
	else
		degrees_F = 0;
}