#include "GlobalDefinitions.h"
#include "RTC.h"
#include "LCD.h"


volatile unsigned char RTC_raw_timer = 61;

void RTC_split_timer(volatile unsigned char raw_time)
{
	
}

void RTC_print_timer(volatile unsigned char binary)
{
	//volatile unsigned char hours = 0;
	volatile unsigned char minutes = 0;
	volatile unsigned char seconds = 0;
	//volatile unsigned char hours10 = 0;
	//volatile unsigned char hours1 = 0;
	volatile unsigned char minutes10 = 0;
	volatile unsigned char minutes1 = 0;
	volatile unsigned char seconds10 = 0;
	volatile unsigned char seconds1 = 0;
	
	minutes = (binary-1)/60;
	minutes10 = minutes/10;
	minutes1 = minutes%10;
	seconds = (binary-1)%60;
	seconds10 = seconds/10;
	seconds1 = seconds%10;
	
	LCD_char(minutes10 + '0');
	LCD_char(minutes1 + '0');
	LCD_char(':');
	LCD_char(seconds10 + '0');
	LCD_char(seconds1 + '0');
}

void RTC_write(volatile unsigned char reg_address, volatile unsigned char data)
{
	TWCR = 0xA4; //start=1, must be cleared in SW after transmission, stop=0, automatically cleared after transmission, clear flag (=1) to send start bit
	while(!checkHIGH(TWCR, TWINT)); //wait for the start bit to be transmitted. Once the start bit is transmitted, TWINT is set by hardware (=0)
	
	TWDR = 0b10100010; //enter the slave address and the write bit (0) into the data register
	TWCR = 0x84; //clear the start bit, clear flag (=1) to send slave address and write bit
	while(!checkHIGH(TWCR, TWINT)); //wait for SLA+W to be transmitted and ACK to be received
	
	TWDR = reg_address; //enter the internal register address of the RTC
	TWCR = 0x84; //clear flag (=1) to send internal register address
	while(!checkHIGH(TWCR, TWINT)); //wait for the internal register address to be transmitted and ACK to be received
	
	TWDR = data; //enter data for internal register
	TWCR = 0x84; //clear flag (=1) to send data
	while(!checkHIGH(TWCR, TWINT)); //wait for the data to be transmitted and ACK to be received
	
	TWCR = 0x94; //start=0, stop=1, automatically cleared after transmission, clear flag (=1) to send stop bit
	while(checkHIGH(TWCR, TWSTO)); //wait for the stop bit to be transmitted (this was the main problem; other problem was setting each TWCR bit one at a time)
}

volatile unsigned int RTC_read(volatile unsigned char reg_address)
{	
	volatile unsigned int data;
	
	//start
	TWCR = 0xA4; //start=1, must be cleared in SW after transmission, stop=0, automatically cleared after transmission, clear flag (=1) to send start bit
	while(!checkHIGH(TWCR, TWINT)); //wait for the start bit to be transmitted. Once the start bit is transmitted, TWINT is set by hardware (=0)

	//slave+W
	TWDR = 0b10100010; //enter the slave address and the write bit (0) into the data register
	TWCR = 0x84; //clear the start bit, clear flag (=1) to send slave address and write bit
	while(!checkHIGH(TWCR, TWINT)); //wait for SLA+W to be transmitted and ACK to be received

	//write reg address
	TWDR = reg_address; //enter the internal register address of the RTC
	TWCR = 0x84; //clear flag (=1) to send internal register address
	while(!checkHIGH(TWCR, TWINT)); //wait for the internal register address to be transmitted and ACK to be received

	//repeated start
	TWCR = 0xA4;
	while(!checkHIGH(TWCR, TWINT)); //wait for the start bit to be transmitted. Once the start bit is transmitted, TWINT is set by hardware (=0)

	//slave+R
	TWDR = 0b10100011; //enter the slave address and the read bit (1) into the data register
	TWCR = 0x84; //clear the start bit, clear flag (=1) to send slave address and read bit
	while(!checkHIGH(TWCR, TWINT)); //wait for SLA+W to be transmitted and ACK to be received

	//receive data
	TWCR = 0x84; //clear flag (=1) to send data
	while(!checkHIGH(TWCR, TWINT)); //wait for the data to be transmitted and ACK to be received
	//seconds = TWDR; //enter data for internal register
	data = TWDR;
	
	//stop
	TWCR = 0x94; //start=0, stop=1, automatically cleared after transmission, clear flag (=1) to send stop bit
	while(checkHIGH(TWCR, TWSTO)); //wait for the stop bit to be transmitted (this was the main problem; other problem was setting each TWCR bit one at a time)
	
	return data;
}