#include "FONA.h"
#include "GlobalDefinitions.h"
#include "UART.h"
#include "RTC.h"
#include "UI.h"
#include "main.h"

volatile unsigned char latitude[11] = {'0'}; //leave an extra spot for null
volatile unsigned char longitude[12] = {'0'};
volatile unsigned char message1[] = {"Warning: Child left in vehicle. Reply 'y' to acknowledge"};
volatile unsigned char message2[] = {"Child is now OK"};
volatile char latitude_count = 0;
volatile char longitude_count = 0;
volatile unsigned char GPS_grab = 0; //0 means we don't have the GPS coordinates, 1 means we have them
// volatile unsigned char FONA_received_message[20] = {'0'};
// volatile unsigned char *FONA__incoming_message_compare = "Message received +SM";
// volatile unsigned char FONA_incoming_message_count = 0;
volatile unsigned char FONA_count = 0;
volatile unsigned char BATTERY_COMMA_COUNT=0;
volatile unsigned char BATTERY_count=0;
volatile unsigned char SIGNAL_STRENGTH_COMMA_COUNT=0;
volatile unsigned char SIGNAL_STRENGTH_COUNT=0;
volatile unsigned char Signal_strength[4];
volatile unsigned char Signal_Strength_Tracker=0;
volatile unsigned char Signal_Flag=0;
volatile unsigned char charge_status[4] = {'0'};
volatile unsigned char charge_status_count = 0;
volatile unsigned char driver_phone_flag=0; 


void send_text(volatile unsigned char recipient[])
{
	asm volatile("nop");
	/*		UART1_TX("AT+CGNSINF\r\n"); //grab GPS coordinates*/
	//UART1_TX("AT+CMGS=\"3057728838\"\r\n");
	UART1_TX("AT+CMGS=\"");
	_delay_ms(25);
	UART1_TX(recipient);
	_delay_ms(25);
	UART1_TX("\"\r\n");
	_delay_ms(25);
	UART1_TX(message1);
	_delay_ms(25);
	UART1_TX("\r\n");
	_delay_ms(25);
	UART1_TX(latitude);
	_delay_ms(25);
	UART1_TX(", ");
	_delay_ms(25);
	UART1_TX(longitude);
	_delay_ms(25);
	send_data(0x1A);
	_delay_ms(25);
	// 		m = 0;
	// 		n = 0;
	// 		GPS_comma_count = 0;
	asm volatile("Nop");
	//_delay_ms(100);
	
}

void send_text_2(volatile unsigned char recipient[], volatile unsigned char message[])
{
	asm volatile("nop");
	if (driver_phone_flag==0)
	{
	 UART1_TX("AT+CMGF=1\r\n"); //Enable SMS - FONA
	 driver_phone_flag=1;
	 _delay_ms(25);
	}
	/*		UART1_TX("AT+CGNSINF\r\n"); //grab GPS coordinates*/
	//UART1_TX("AT+CMGS=\"3057728838\"\r\n");
	UART1_TX("AT+CMGS=\"");
	_delay_ms(25);
	UART1_TX(recipient);
	_delay_ms(25);
	UART1_TX("\"\r\n");
	_delay_ms(25);
	UART1_TX(message);
	_delay_ms(25);
	UART1_TX("\r\n");
	_delay_ms(25);
	UART1_TX(latitude);
	_delay_ms(25);
	UART1_TX(", ");
	_delay_ms(25);
	UART1_TX(longitude);
	_delay_ms(25);
	send_data(0x1A);
	_delay_ms(25);
	// 		m = 0;
	// 		n = 0;
	// 		GPS_comma_count = 0;
	asm volatile("Nop");
	//_delay_ms(100);
	
}

void send_text_3(volatile unsigned char recipient[], volatile unsigned char message[])
{
// 	asm volatile("nop");
// 	if (driver_phone_flag==0)
// 	{
// 		UART1_TX("AT+CMGF=1\r\n"); //Enable SMS - FONA
// 		// driver_phone_flag=1;
// 		_delay_ms(25);
// 	}
	/*		UART1_TX("AT+CGNSINF\r\n"); //grab GPS coordinates*/
	//UART1_TX("AT+CMGS=\"3057728838\"\r\n");
	UART1_TX("AT+CMGS=\"");
	_delay_ms(25);
	UART1_TX(recipient);
	_delay_ms(25);
	UART1_TX("\"\r\n");
	_delay_ms(25);
	UART1_TX(message);
	_delay_ms(25);
	UART1_TX("\r\n");
	_delay_ms(25);
	UART1_TX(latitude);
	_delay_ms(25);
	UART1_TX(", ");
	_delay_ms(25);
	UART1_TX(longitude);
	_delay_ms(25);
	send_data(0x1A);
	_delay_ms(25);
	// 		m = 0;
	// 		n = 0;
	// 		GPS_comma_count = 0;
	asm volatile("Nop");
	//_delay_ms(100);
	
}

void send_text_4(volatile unsigned char recipient[], volatile unsigned char message[])
{
// 	asm volatile("nop");
// 	if (driver_phone_flag==0)
// 	{
// 		UART1_TX("AT+CMGF=1\r\n"); //Enable SMS - FONA
// 		// driver_phone_flag=1;
// 		_delay_ms(25);
// 	}
	/*		UART1_TX("AT+CGNSINF\r\n"); //grab GPS coordinates*/
	//UART1_TX("AT+CMGS=\"3057728838\"\r\n");
	UART1_TX("AT+CMGS=\"");
	_delay_ms(25);
	UART1_TX(recipient);
	_delay_ms(25);
	UART1_TX("\"\r\n");
	_delay_ms(25);
	UART1_TX(message);
	_delay_ms(25);
	UART1_TX("\r\n");
	_delay_ms(25);
	UART1_TX(latitude);
	_delay_ms(25);
	UART1_TX(", ");
	_delay_ms(25);
	UART1_TX(longitude);
	_delay_ms(25);
	send_data(0x1A);
	_delay_ms(25);
	// 		m = 0;
	// 		n = 0;
	// 		GPS_comma_count = 0;
	asm volatile("Nop");
	//_delay_ms(100);
	
}

// void DISPLAY_BATTERY_STATUS(void)
// {
// 	LCD_command(0x51, 0); //clear LCD
// 	LCD_command(0x4C, 0); //turn off blinking cursor
// 	LCD_command(0x46, 0); //cursor home
// 	LCD_string_P(PSTR("Please Recharge Battery"));
// 	LCD_command(0x45, 0x40); //second line
// 	
// }
// 
// void WARN_OF_LOW_COVERAGE(void)
// {
// 	LCD_command(0x51, 0); //clear LCD
// 	LCD_command(0x4C, 0); //turn off blinking cursor
// 	LCD_command(0x46, 0); //cursor home
// 	LCD_string_P(PSTR("Warning:Bad Signal"));
// 	LCD_command(0x45, 0x40); //second line
// }

//void DISPLAY_STATUS(void)
//{
	//LCD_command(0x51, 0); //clear LCD
	//LCD_command(0x4C, 0); //turn off blinking cursor
	//LCD_command(0x46, 0); //cursor home
	//
	//LCD_string_P(PSTR("Battery: "));
	//LCD_string(charge_status);
	//LCD_string_P(PSTR("%"));
	//
	//LCD_command(0x45, 0x40); //second line
	//LCD_string_P(PSTR("Signal: "));
	//LCD_string(Signal_strength);
	//LCD_string_P(PSTR(" dB"));
//}