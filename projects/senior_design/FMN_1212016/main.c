#define F_CPU 1000000
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <avr/eeprom.h>
#include <avr/pgmspace.h>


#include "GlobalDefinitions.h"
#include "UI.h"
#include "main.h"
#include "SPI.h"
#include "UART.h"
#include "Bluetooth.h"
#include "LCD.h"
#include "keypad.h"
#include "I2C.h"
#include "RTC.h"
#include "timers.h"
#include "ADC.h"
#include "temperature.h"
#include "FONA.h"
#include "LED.h"
#include "DAC.h"

int main(void)
{
	setHIGH(DDRA, PA0); //LED
	setLOW(PORTA, PA0);
	setHIGH(DDRA, PA5); //fan
	setHIGH(DDRB, PB1); //DAC SS output set
	setHIGH(PORTB, PB1); //DAC SS disabled
	SPI_init(); //initialize SPI (LCD default settings)
	LCD_init(); //initialize the LCD (including SPI registers)
	_delay_ms(1000); //gives LCD, FONA808, and RN41 time to boot up (1 full second)
	LCD_command(0x41, 0); //display on
	LCD_command(0x53, 0x02); //set brightness
	LCD_command(0x51, 0); //clear LCD
	LCD_command(0x46, 0); //cursor home
	LCD_command(0x4C, 0); //turn off blinking cursor
	
	LCD_string("   Forget Me Not");
	LCD_command(0x45, 0x40); //second line
	LCD_string("  (Team Gatoraid)  ");
	LCD_command(0x45, 0x14); //third line
	LCD_string("    Nikolas Horn ");
	LCD_command(0x45, 0x54); //fourth line
	LCD_string("  Mackenzie Sawall ");
	_delay_ms(1500);
	
	ADC_init();
	UART1_init(); //initialize UART1
	UART0_init(); //initialize UART0
	keypad_init(); //initialize keypad (interrupts, SS)
	timer1_init();
	PWM_init();
	I2C_init();
	//Bluetooth_init(); //initialize Bluetooth and make  RN41 quiet (non-discoverable) at startup
	
// 	UART0_TX("$$$"); //enter command mode
// 	_delay_ms(100);
// 	UART0_TX("Q\r"); //discovery disabled (Quiet mode)
// 	_delay_ms(100);
// 	UART0_TX("---\r"); //exit command mode
// 	_delay_ms(100);

	
	UI_top_menu();

	RTC_write(0x01, 0b00000000); //initialize timer settings
	RTC_write(0x10, 0b01000010); //enable timer at 1 Hz
	RTC_write(0x11, 0); //load timer with 0 to stop it
	
	UI_copy_numbers();
	asm volatile("Nop");
	sei(); //enable interrupts (had to be moved down here instead of first because ISR was being called too soon
	
	//BT_connection_flag = 0;
	//BT_connection_query();
	main_processes(); //pressing 'D' will escape to here

	return 0;
}

void main_processes(void)
{
	while(1)
	{				
		if(menu_flag == 1)
		{
			UI_menu(); //access menu
			menu_flag = 0;
		}
		else if(view_flag == 1)
		{
			UI_view(); //access stored phone numbers
			view_flag = 0;
		}
		else if(background_flag == 1)
		{
			UI_background(); //access countdown timers
			background_flag = 0;
		}
		else if(status_flag == 1)
		{
			UI_status(); //access battery status and signal strength
			status_flag = 0;
		}
	}
}

ISR(INT2_vect)
{
	setHIGH(EIFR, INTF2); //clears the flag for INT2
	
	if(menu_flag == 0 && view_flag == 0 && background_flag == 0 && status_flag == 0) //only read keypad through SPI if all flags are 0
	{
		//read the keypad input without waiting for another press...
		keypad_SPIreset();
		SPDR = SPI_received_byte; //initiate transfer?
		while(!checkHIGH(SPSR, SPIF)); //wait until transfer complete
		SPI_received_byte = SPDR;
		
		if(SPI_received_byte == '*')
		{
			menu_flag = 1;
			view_flag = 0;
			background_flag = 0;
			status_flag = 0;
		}
		
		if(SPI_received_byte == '#')
		{
			menu_flag = 0;
			view_flag = 1;
			background_flag = 0;
			status_flag = 0;
		}
		
		if(SPI_received_byte == 'C')
		{
			menu_flag = 0;
			view_flag = 0;	
			background_flag = 1;
			status_flag = 0;		
		}
		
		if(SPI_received_byte == 'A')
		{
			menu_flag = 0;
			view_flag = 0;
			background_flag = 0;
			status_flag = 1;
		}
		
// 		if(SPI_received_byte == 'D' && BT_connection_flag == 0) //cancel all operations manually
// 		{
// 			RTC_write(0x11, 0); //load timer with 0 to stop it
// 			setLOW(TIMSK1, OCIE1A); //disable timer1 Output Compare Match A interrupts
// 			RTC_raw_timer = total_time;
// 			SMS_driver_flag = 0;
// 			SMS_EC1_flag = 0;
// 			SMS_EC2_flag = 0;
// 			SMS_911_flag = 0;
// 		}
	}
	
	else //if any key is pressed and one or more flags are set, clear all flags
	{
		//menu_flag = 0;
		view_flag = 0;
		background_flag = 0;
		status_flag = 0;
		
		if(menu_flag == 1 && BT_discovery_flag == 1)
		{
			BT_discovery_flag = 0;
			menu_flag = 0;
		}
	}
	

}

ISR (USART0_RX_vect) //RN41 Bluetooth
{
	setHIGH(UCSR0A, RXC0); //clear flag
	UART0_received_byte = UDR0;
	
	if(UART0_received_byte == 'i')
		BT_status_count = 0;
	BT_status[BT_status_count] = UART0_received_byte;
	BT_status_count++;
	if(BT_status_count >= 10)
	{
		BT_status_count = 0;
		if(BT_status[0] == 'i')
			BT_connection_query(); //poll for a disconnect or connect	
	}
	
	//UARTtoLCD(UART0_received_byte);
}

ISR (TIMER1_COMPA_vect) //every 1 second, read the I2C count value if BT_connection_flag = 0
{
	setHIGH(TIFR1, OCF1A); //clear timer1 compare flag
	TCNT1 = 0; //reset count to 0
	toggle(PORTA, PA0);
	//sinewave_out();
	
	setHIGH(ADCSRA, ADSC); //start conversion
	temperature = ADC;
	temperature_check(temperature);
	
	if(degrees_F == 1)
	{
		setHIGH(PORTA, PA5); //turn on fan
	}
	else
	{
		setLOW(PORTA, PA5); //turn off fan
	}
	
	if (BT_connection_flag == 0) //if phone becomes disconnected
	{
		RTC_raw_timer = RTC_read(0x10); //read countdown timer raw value
		
		
		if (RTC_raw_timer>(total_time - (total_time/4)*1))
		{
			red();
		}
		if(RTC_raw_timer <= (total_time - (total_time/4)*1))
		{
			
			SMS_driver_flag = 1;
			if (FONA_FLAG==0)
			{
				blue();
				send_text(driver_phone);
				FONA_FLAG++;
			}
		}
		if(RTC_raw_timer <= (total_time - (total_time/4)*2))
		{
			
			SMS_EC1_flag = 1;
			if (FONA_FLAG==1)
			{
				green();
				send_text(emergency1_phone);
				FONA_FLAG++;
			}
		}	
		
		if(RTC_raw_timer <= (total_time - (total_time/4)*3))
		{
			SMS_EC2_flag = 1;
			if (FONA_FLAG==2)
			{
				purple();
				send_text(emergency2_phone);
				FONA_FLAG++;
			}
		}
		
		
		if((RTC_raw_timer == 1))
		{
			no_color();
			RTC_write(0x11, 0); //stop the timer
			if (toggle_911==1)
			{
				no_color();
			    if (FONA_FLAG==3)
				{
					no_color();
					SMS_911_flag = 1;
					send_text(driver_phone);
					FONA_FLAG++;
				}
			}
			
		    
		}
		
		
	} 
	
	if (BT_connection_flag == 1) //if phone becomes connected
	{
		background_flag = 0;
		RTC_raw_timer = 241; //reset timer to 4 minutes
		SMS_driver_flag = 0;
		SMS_EC1_flag = 0;
		SMS_EC2_flag = 0;
		SMS_911_flag = 0;
		latitude_count = 0;
		longitude_count = 0;
		GPS_comma_count = 0;
		GPS_grab = 0;
		//driver_phone_flag=0;
		FONA_count = 0;
		no_color();
	}
	
	//RTC_read(0x11); //read countdown timer and set equal to RTC_raw_timer
}

 ISR (USART1_RX_vect)
 {
 	setHIGH(UCSR1A, RXC1); //clear flag
	UART1_received_byte = UDR1;

	if(GPS_grab == 0)
	{
		if (UART1_received_byte == ',')
		{
			GPS_comma_count++;
			asm volatile("Nop");
			if(GPS_comma_count > 4)
			{
				GPS_grab = 1;
			}
		}
		
		else if(UART1_received_byte != ',')
		{
			if (GPS_comma_count == 3)
			{
				latitude[latitude_count] = UART1_received_byte;
				latitude_count++;
			}
			
			if(GPS_comma_count == 4)
			{
				longitude[longitude_count] = UART1_received_byte;
				longitude_count++;
			}
		}
	}

	else if (GPS_grab == 1)
	{
		asm volatile("Nop");
		if(UART1_received_byte == '"' && FONA_count == 0)
			FONA_count++;
		else if(UART1_received_byte == 'S' && FONA_count == 1)
			FONA_count++;
		else if(UART1_received_byte == 'M' && FONA_count == 2)
			FONA_count++;
		else if(UART1_received_byte == '"' && FONA_count == 3)
			FONA_count++;
		else if(UART1_received_byte == ',' && FONA_count == 4)
			FONA_count++;
		else if(UART1_received_byte == '1' && FONA_count == 5)
		{
			FONA_count = 0;
			BT_connection_flag = 1;
			UART1_TX("AT+CMGD=30,4\r\n"); //delete all sms messages
			_delay_ms(30);
			send_text_2(driver_phone, "TRIGGERED");
			//driver_phone_flag=1; 
			asm volatile("nop");
			send_text_2(emergency1_phone, "TRIGGERED");
			send_text_2(emergency2_phone, "TRIGGER");
			driver_phone_flag=0;
			_delay_ms(100);
			setLOW(PORTD, PD4); //turn off Fona Reference Voltage
		}
	}
	
	if (status_flag == 1)
	{
		//polling code for battery charge
		if ((UART1_received_byte=='+')&&(BATTERY_count==0))
			BATTERY_count++;
		else if ((UART1_received_byte=='C')&&(BATTERY_count==1))
			BATTERY_count++;
		else if ((UART1_received_byte=='B')&&(BATTERY_count==2))
			BATTERY_count++;
		else if ((UART1_received_byte=='C')&&(BATTERY_count==3))
			BATTERY_count++;
			
		if (UART1_received_byte==',' && BATTERY_count == 4)
		{
			BATTERY_COMMA_COUNT++;
		}
			
		else if ((UART1_received_byte!=',')&&(BATTERY_COMMA_COUNT==1))
		{
			charge_status[charge_status_count] = UART1_received_byte;
			charge_status_count++;
		}
		
	
		//SIGNAL_STRENGTH_COUNT = 0;
		//polling code for Signal Strength
		if ((UART1_received_byte=='+')&&(SIGNAL_STRENGTH_COUNT==0))
			SIGNAL_STRENGTH_COUNT++;
		else if ((UART1_received_byte=='C')&&(SIGNAL_STRENGTH_COUNT==1))
			SIGNAL_STRENGTH_COUNT++;
		else if ((UART1_received_byte=='S')&&(SIGNAL_STRENGTH_COUNT==2))
			SIGNAL_STRENGTH_COUNT++;
		else if ((UART1_received_byte=='Q')&&(SIGNAL_STRENGTH_COUNT==3))
			SIGNAL_STRENGTH_COUNT++;
		else if ((UART1_received_byte==':')&&(SIGNAL_STRENGTH_COUNT==4))
			SIGNAL_STRENGTH_COUNT++;
		else if ((UART1_received_byte==' ')&&(SIGNAL_STRENGTH_COUNT==5))
		{
			SIGNAL_STRENGTH_COUNT++;
			Signal_Flag = 0;
		}
		if ((UART1_received_byte!=',')&&(Signal_Flag!=1) && (SIGNAL_STRENGTH_COUNT == 6))
		{
			Signal_strength[Signal_Strength_Tracker]=UART1_received_byte;
			Signal_Strength_Tracker++;
		}
		else if (UART1_received_byte==',')
		{
			Signal_Flag=1;
		}

	}

} 

ISR (ADC_vect)
{
	setHIGH(ADCSRA, ADIF); //clear ADC complete interrupt flag
	temperature = ADC;
}


	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	

