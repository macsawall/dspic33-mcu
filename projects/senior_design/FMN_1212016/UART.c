#include "UART.h"
#include "GlobalDefinitions.h"
#include "LCD.h"
#include "main.h"

volatile unsigned char UART1_received_byte = 'G';
volatile unsigned char UART0_received_byte = 'G';
volatile unsigned char GPS_comma_count = 0;
volatile unsigned char UART1_flag = 0; 
volatile unsigned char FONA_FLAG = 0;

void UART1_init(void)
{
	setLOW(DDRD, PD2); //pin PD2 set as input for UART RX1
	setHIGH(DDRD, PD3); //pin PD3 set as output for UART TX1
	setHIGH(UCSR1A, U2X1); //double USART1 transmission speed
	setHIGH(UCSR1B, RXEN1); //enable RX0
	setHIGH(UCSR1B, TXEN1); //enable TX0
	setHIGH(UCSR1B, RXCIE1); // enable UART1 RX complete interrupts
	//setHIGH(UCSR1B, TXCIE1); // enable UART1 TX complete interrupts
	setLOW(UCSR1C, UMSEL11); //set to asynchronous
	setLOW(UCSR1C, UMSEL10); //
	setLOW(UCSR1C, UPM11); //set to no parity
	setLOW(UCSR1C, UPM10); //
	setLOW(UCSR1C, USBS1); //set to 1 stop bit
	setLOW(UCSR1C, UCSZ12); //set data to 8 bits
	setHIGH(UCSR1C, UCSZ11); //
	setHIGH(UCSR1C, UCSZ10); //
	setLOW(UCSR1C, UCPOL1); // set to 0 for asynchronous mode (clock polarity)
	UBRR1H = 0b00000000; // set baud rate to 9600
	UBRR1L = 0b00001100; //
	setHIGH(DDRD, PD4); //FONA Vio 
}

void UART0_init(void)
{
	setLOW(DDRD, PD0); //pin PD0 set as input for UART RX0
	setHIGH(DDRD, PD1); //pin PD1 set as output for UART TX0
	setHIGH(UCSR0A, U2X0); //double USART transmission speed
	setHIGH(UCSR0B, RXEN0); //enable RX0
	setHIGH(UCSR0B, TXEN0); //enable TX0
	setHIGH(UCSR0B, RXCIE0); // enable UART0 RX complete interrupts
	//setHIGH(UCSR0B, TXCIE0); // enable UART0 TX complete interrupts
	setLOW(UCSR0C, UMSEL01); //set to asynchronous
	setLOW(UCSR0C, UMSEL00); //
	setLOW(UCSR0C, UPM01); //set to no parity
	setLOW(UCSR0C, UPM00); //
	setLOW(UCSR0C, USBS0); //set to 1 stop bit
	setLOW(UCSR0C, UCSZ02); //set data to 8 bits
	setHIGH(UCSR0C, UCSZ01); //
	setHIGH(UCSR0C, UCSZ00); //
	setLOW(UCSR0C, UCPOL0); // set to 0 for asynchronous mode (clock polarity)
	UBRR0H = 0b00000000; // set baud rate to 9600
	UBRR0L = 0b00001100; //
}

void UART1_TX(volatile unsigned char string[])
{
	int i = 0;
	
		while(string[i] != 0) //loop though string until all characters written
	{
		while(!checkHIGH(UCSR1A, UDRE1));
		UDR1 = string[i];
		i++;
	}
	
	
	
}

void UART0_TX(volatile unsigned char string[])
{
	int i = 0;
	while(string[i] != 0) //loop though string until all characters written
	{
		while(!checkHIGH(UCSR0A, UDRE0));
		UDR0 = string[i];
		i++;
	}
}

void UARTtoLCD(volatile unsigned char key) //This function echoes UART/RN41 info to LCD. Parameter is UART0 received byte from RN41.
{
	while(checkHIGH(UCSR0A, TXC0));
	//UDR0 = key; //echo back to putty/RN41 (messes things up when echoing to LCD, don't use unless using putty. Maybe use UART1)
	
	if(key == '\n' || key == '\r') // carriage return character(s)
	//do nothing; don't echo anything to LCD because it prints garbage
		asm volatile("nop");
	else
		LCD_char(key); //echo characters to LCD that are sent to RN41
}

void send_data(volatile unsigned char rn41)
{
	while ((UCSR1A & (1 << UDRE1)) == 0) {};
	UDR1 = rn41;
}

