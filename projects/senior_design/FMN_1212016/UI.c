#include "GlobalDefinitions.h"
#include "main.h"
#include "SPI.h"
#include "UART.h"
#include "Bluetooth.h"
#include "LCD.h"
#include "keypad.h"
#include "UI.h"
#include "I2C.h"
#include "RTC.h"
#include "timers.h"
#include "ADC.h"
#include "temperature.h"
#include "FONA.h"

volatile unsigned char driver_phone[11] = {0};
volatile unsigned char emergency1_phone[11] = {0};
volatile unsigned char emergency2_phone[11] = {0};
volatile unsigned int menu_flag = 0;
volatile unsigned int BT_discovery_flag = 0;
volatile unsigned int view_flag = 0;
volatile unsigned int background_flag = 0;
volatile unsigned int status_flag = 0;
volatile unsigned char toggle_911 = 0;

volatile unsigned int SMS_driver_flag = 0;
volatile unsigned int SMS_EC1_flag = 0;
volatile unsigned int SMS_EC2_flag = 0;
volatile unsigned int SMS_911_flag = 0;


void UI_menu(void)
{
	setLOW(EIMSK, INT2); //disable external interrupts on INT2 while accessing menu because it screws with keypad input
	while(SPI_received_byte != 'D')
	{
		LCD_command(0x51, 0); //clear LCD
		LCD_command(0x4C, 0); //turn off blinking cursor
		LCD_command(0x46, 0); //cursor home
		/*LCD_string(menu_strings[0]);*/
		LCD_string_P(PSTR("(A) Enter phone #s"));
		LCD_command(0x45, 0x40); //second line
		LCD_string_P(PSTR("(B) Set up Bluetooth"));
		LCD_command(0x45, 0x14); //third line
		LCD_string_P(PSTR("(C) Toggle 911 SMS"));
		LCD_command(0x45, 0x54); //fourth line
		LCD_string_P(PSTR("(D) Exit menu"));
		keypad_read(); //menu gets stuck (on purpose) until A, B, C, or D is pressed
		
		////////////////////////// MENU SELECTION A //////////////////////////////////
		if(SPI_received_byte == 'A')
		{
			//Driver #
			LCD_command(0x51, 0); //clear LCD
			LCD_command(0x46, 0); //cursor home
			LCD_string_P(PSTR("Enter your 10 digit"));
			LCD_command(0x45, 0x40); //second line
			LCD_string_P(PSTR("phone number, then"));
			LCD_command(0x45, 0x14); //third line
			LCD_string_P(PSTR("press #:"));
			LCD_command(0x45, 0x54); //fourth line
			UI_store_number(driver_phone, 0);
			
			//Emergency contact 1 #
			LCD_command(0x51, 0); //clear LCD
			LCD_command(0x46, 0); //cursor home
			LCD_string_P(PSTR("Enter 10 digit # of"));
			LCD_command(0x45, 0x40); //second line
			LCD_string_P(PSTR("emergency contact 1,"));
			LCD_command(0x45, 0x14); //third line
			LCD_string_P(PSTR("then press #:"));
			LCD_command(0x45, 0x54); //fourth line
			UI_store_number(emergency1_phone, 10);
			
			//Emergency contact 2 #
			LCD_command(0x51, 0); //clear LCD
			LCD_command(0x46, 0); //cursor home
			LCD_string_P(PSTR("Enter 10 digit # of"));
			LCD_command(0x45, 0x40); //second line
			LCD_string_P(PSTR("emergency contact 2,"));
			LCD_command(0x45, 0x14); //third line
			LCD_string_P(PSTR("then press #:"));
			LCD_command(0x45, 0x54); //fourth line
			UI_store_number(emergency2_phone, 20);
			
			ESCAPE();
		}
		
		////////////////////////// MENU SELECTION B //////////////////////////////////
		else if(SPI_received_byte == 'B')
		{
			asm volatile("Nop");
			if(BT_connection_flag == 1)
			{
				LCD_command(0x51, 0); //clear LCD
				LCD_command(0x4C, 0); //turn off blinking cursor
				LCD_command(0x46, 0); //cursor home
				LCD_string_P(PSTR("  Forget Me Not is"));
				LCD_command(0x45, 0x40); //second line
				LCD_string_P(PSTR("  already connected"));
				LCD_command(0x45, 0x14); //third line
				LCD_string_P(PSTR("    to a device!"));
				_delay_ms(3000); //wait 3 seconds before clearing message from screen
			}
			else //if BT_connection_flag is 0 or 2 (disconnected)
			{
				setHIGH(EIMSK, INT2); //enable external interrupts on INT2
				LCD_command(0x51, 0); //clear LCD
				LCD_command(0x4C, 0); //turn off blinking cursor
				LCD_command(0x46, 0); //cursor home
				LCD_string_P(PSTR("    Discoverable    "));
				LCD_command(0x45, 0x14); //third line
				for(int i=0; i<8; i++)
					LCD_command(0x4A, 0); //cursor right
				
				//make RN41 discoverable, then reboot
				//Bluetooth_init(); //one or more of the commands messes up our stored compare strings somehow!?
				//asm volatile("Nop"));
				UART0_TX("$$$"); //enter command mode
				_delay_ms(100);
				UART0_TX("Q,0\r"); // discovery enabled
				_delay_ms(100);
				UART0_TX("---\r"); //exit command mode
				_delay_ms(100);
				
				BT_connection_flag = 2;
				BT_discovery_flag = 1;
				//wait until a phone connects to RN41
				while(BT_connection_flag != 1) //while not connected
				{
					UI_loading_animation();
					if(BT_discovery_flag == 0)
						ESCAPE();
				}
				setLOW(EIMSK, INT2); //disable external interrupts on INT2 while accessing menu because it screws with keypad input
				LCD_command(0x51, 0); //clear LCD
				LCD_command(0x4C, 0); //turn off blinking cursor
				LCD_command(0x46, 0); //cursor home
				LCD_string_P(PSTR("     Connected!"));
				_delay_ms(3000); //wait 3 seconds before clearing message from screen
			}
			ESCAPE();
		}
		
		////////////////////////// MENU SELECTION C //////////////////////////////////
		else if(SPI_received_byte == 'C')
		{
			LCD_command(0x51, 0); //clear LCD
			LCD_command(0x4C, 0); //turn off blinking cursor
			LCD_command(0x46, 0); //cursor home
			LCD_string_P(PSTR("Is 911 SMS messaging"));
			LCD_command(0x45, 0x40); //second line
			LCD_string_P(PSTR("available in your"));
			LCD_command(0x45, 0x14); //third line
			LCD_string_P(PSTR("area?"));
			LCD_command(0x45, 0x54); //fourth line
			LCD_string_P(PSTR("  (A) Yes   (B) No  "));
			
			while(menu_flag == 1)
			{
				keypad_read();
				
				if(SPI_received_byte == 'A')
				{
					LCD_command(0x51, 0); //clear LCD
					LCD_command(0x4C, 0); //turn off blinking cursor
					LCD_command(0x46, 0); //cursor home
					//read EEPROM for toggle_911
					while(checkHIGH(EECR, EEPE)); //wait for EEPROM write enable to become 0 (make sure a write operation is not occurring because you can't read and write simultaneously
					EEAR = 30; //EEPROM addresses 0-9 (out of 511)
					setHIGH(EECR, EERE); //Read enable
					if(EEDR == 1)
						LCD_string_P(PSTR("   911 SMS is ON"));
					else //if(EEDR == 0)
						LCD_string_P(PSTR("   911 SMS is OFF"));
					LCD_command(0x45, 0x14); //third line
					LCD_string_P(PSTR("(A) Enable 911 SMS"));
					LCD_command(0x45, 0x54); //fourth line
					LCD_string_P(PSTR("(B) Disable 911 SMS"));
					
					while(menu_flag == 1)
					{
						keypad_read();
						
						if(SPI_received_byte == 'A')
						{
							toggle_911 = 1;
							//write toggle_911 to EEPROM
							while(checkHIGH(EECR, EEPE)); //wait for EEPROM write enable to become 0
							EEAR = 30; //EEPROM address 30 (out of 511)
							EEDR = toggle_911; //EEPROM data
							EECR = 0b00000100; //Enable master. At same time disable write enable
							setHIGH(EECR, EEPE); //Enable EEPROM write enable
							LCD_command(0x51, 0); //clear LCD
							LCD_command(0x4C, 0); //turn off blinking cursor
							LCD_command(0x46, 0); //cursor home
							LCD_string_P(PSTR("  911 SMS enabled   "));
							_delay_ms(3000);
							ESCAPE();
						}
						
						else if(SPI_received_byte == 'B')
						{
							toggle_911 = 0;
							//write toggle_911 to EEPROM
							while(checkHIGH(EECR, EEPE)); //wait for EEPROM write enable to become 0
							EEAR = 30; //EEPROM address 30 (out of 511)
							EEDR = toggle_911; //EEPROM data
							EECR = 0b00000100; //Enable master. At same time disable write enable
							setHIGH(EECR, EEPE); //Enable EEPROM write enable
							LCD_command(0x51, 0); //clear LCD
							LCD_command(0x4C, 0); //turn off blinking cursor
							LCD_command(0x46, 0); //cursor home
							LCD_string_P(PSTR("  911 SMS disabled  "));
							_delay_ms(3000);
							ESCAPE();
						}
						
						else if(SPI_received_byte == 'D')
							ESCAPE();
					}
					
				}
				
				else if(SPI_received_byte == 'B')
				{
					toggle_911 = 0;
					//write toggle_911 to EEPROM
					while(checkHIGH(EECR, EEPE)); //wait for EEPROM write enable to become 0
					EEAR = 30; //EEPROM address 30 (out of 511)
					EEDR = toggle_911; //EEPROM data
					EECR = 0b00000100; //Enable master. At same time disable write enable
					setHIGH(EECR, EEPE); //Enable EEPROM write enable
					LCD_command(0x51, 0); //clear LCD
					LCD_command(0x4C, 0); //turn off blinking cursor
					LCD_command(0x46, 0); //cursor home
					LCD_string_P(PSTR("  911 SMS disabled  "));
					_delay_ms(3000);
					ESCAPE();
				}
				
				else if(SPI_received_byte == 'D')
					ESCAPE();
			}
			
		}	
	}
	ESCAPE();
}

void UI_view(void)
{
	//setLOW(EIMSK, INT2); //disable external interrupts on INT2 while viewing stored numbers because it screws with keypad input
	LCD_command(0x51, 0); //clear LCD
	LCD_command(0x4C, 0); //turn off blinking cursor
	LCD_command(0x46, 0); //cursor home
	LCD_string("#1: ");
	for(int i=0; i<10; i++)
	{
		while(checkHIGH(EECR, EEPE)); //wait for EEPROM write enable to become 0 (make sure a write operation is not occurring because you can't read and write simultaneously
		EEAR = i; //EEPROM addresses 0-9 (out of 511)
		setHIGH(EECR, EERE); //Read enable
		LCD_char(EEDR);
	}
	LCD_command(0x45, 0x40); //second line
	LCD_string("#2: ");
	for(int i=10; i<20; i++)
	{
		while(checkHIGH(EECR, EEPE)); //wait for EEPROM write enable to become 0 (make sure a write operation is not occurring because you can't read and write simultaneously
		EEAR = i; //EEPROM addresses 0-9 (out of 511)
		setHIGH(EECR, EERE); //Read enable
		LCD_char(EEDR);
	}
	LCD_command(0x45, 0x14); //third line
	LCD_string("#3: ");
	for(int i=20; i<30; i++)
	{
		while(checkHIGH(EECR, EEPE)); //wait for EEPROM write enable to become 0 (make sure a write operation is not occurring because you can't read and write simultaneously
		EEAR = i; //EEPROM addresses 0-9 (out of 511)
		setHIGH(EECR, EERE); //Read enable
		LCD_char(EEDR);
	}
	LCD_command(0x45, 0x54); //fourth line
	LCD_string("BT: ");
	while(view_flag == 1)
	{
		if(BT_connection_flag == 0) //if not connected to a phone 
		{
			LCD_command(0x45, 0x54); //fourth line
// 			for(int i=0; i<4; i++)
// 				LCD_command(0x4A, 0); //cursor right
			LCD_string("BT: Disconnected   ");
		}
		
		else if(BT_connection_flag == 1) //if connected to a phone
		{
			LCD_command(0x45, 0x54); //fourth line
// 			for(int i=0; i<4; i++)
// 				LCD_command(0x4A, 0); //cursor right
			LCD_string("BT: Connected      ");
		}
		
		else if(BT_connection_flag == 2) //
		{
			LCD_command(0x45, 0x54); //fourth line
// 			for(int i=0; i<4; i++)
// 				LCD_command(0x4A, 0); //cursor right
			LCD_string("BT: ?              ");
		}
	}
	ESCAPE();
}

void UI_background(void)
{
	LCD_command(0x51, 0); //clear LCD
	LCD_command(0x4C, 0); //turn off blinking cursor
	LCD_command(0x46, 0); //cursor home
	
	LCD_string("Driver: ");
	LCD_command(0x45, 0x40); //second line
	LCD_string("EC1: ");
	LCD_command(0x45, 0x14); //third line
	LCD_string("EC2: ");
	LCD_command(0x45, 0x54); //fourth line
	LCD_string("911: ");
	
	
 	while(background_flag == 1)
 	{
		if(BT_connection_flag == 1 || BT_connection_flag == 2) //if phone is connected or it was never connected
		{
			LCD_command(0x46, 0); //cursor home
			for(int i=0; i<8; i++)
			LCD_command(0x4A, 0); //cursor right
			RTC_print_timer((total_time/4)+1);
			for(int i=0; i<3; i++)
			LCD_command(0x4A, 0); //cursor right
			LCD_string("    ");
			
			LCD_command(0x45, 0x40); //second line
			for(int i=0; i<8; i++)
			LCD_command(0x4A, 0); //cursor right
			RTC_print_timer((total_time/4)+1);
			for(int i=0; i<3; i++)
			LCD_command(0x4A, 0); //cursor right
			LCD_string("    ");
			
			LCD_command(0x45, 0x14); //third line
			for(int i=0; i<8; i++)
			LCD_command(0x4A, 0); //cursor right
			RTC_print_timer((total_time/4)+1);
			for(int i=0; i<3; i++)
			LCD_command(0x4A, 0); //cursor right
			LCD_string("    ");
			
			LCD_command(0x45, 0x54); //fourth line
			for(int i=0; i<8; i++)
			LCD_command(0x4A, 0); //cursor right
			RTC_print_timer((total_time/4)+1);
			for(int i=0; i<3; i++)
			LCD_command(0x4A, 0); //cursor right
			LCD_string("    ");
		}
		
		else if(BT_connection_flag == 0) //if phone is disconnected
		{
				LCD_command(0x46, 0); //cursor home
				for(int i=0; i<8; i++)
				LCD_command(0x4A, 0); //cursor right
				if(RTC_raw_timer > (total_time - (total_time/4)*1) && RTC_raw_timer <= total_time)
					RTC_print_timer(RTC_raw_timer - ((total_time/4)*3));
				else
					LCD_string("00:00");
				if(SMS_driver_flag == 1)
				{
					for(int i=0; i<3; i++)
					LCD_command(0x4A, 0); //cursor right
					LCD_string("Sent");
				}
				
				LCD_command(0x45, 0x40); //second line
				for(int i=0; i<8; i++)
				LCD_command(0x4A, 0); //cursor right
				if(RTC_raw_timer > (total_time - (total_time/4)*2) && RTC_raw_timer <= ((total_time - (total_time/4)*1)-1))
					RTC_print_timer(RTC_raw_timer - ((total_time/4)*2));
				else if(RTC_raw_timer <= (total_time - (total_time/4)*2))
					LCD_string("00:00");
				else
					RTC_print_timer((total_time/4)+1);
				if(SMS_EC1_flag == 1)
				{
					for(int i=0; i<3; i++)
					LCD_command(0x4A, 0); //cursor right
					LCD_string("Sent");
				}
				
				LCD_command(0x45, 0x14); //third line
				for(int i=0; i<8; i++)
				LCD_command(0x4A, 0); //cursor right
				if(RTC_raw_timer > (total_time - (total_time/4)*3) && RTC_raw_timer <= ((total_time - (total_time/4)*2)-1))
					RTC_print_timer(RTC_raw_timer - ((total_time/4)*1));
				else if(RTC_raw_timer <= (total_time - (total_time/4)*3))
					LCD_string("00:00");
				else
					RTC_print_timer((total_time/4)+1);
				if(SMS_EC2_flag == 1)
				{
					for(int i=0; i<3; i++)
					LCD_command(0x4A, 0); //cursor right
					LCD_string("Sent");
				}
				
				LCD_command(0x45, 0x54); //fourth line
				for(int i=0; i<8; i++)
				LCD_command(0x4A, 0); //cursor right
				if(RTC_raw_timer > 1 && RTC_raw_timer <= ((total_time - (total_time/4)*3)-1))
					RTC_print_timer(RTC_raw_timer);
				else if(RTC_raw_timer <= 1)
					LCD_string("00:00");
				else
					RTC_print_timer((total_time/4)+1);
				if(SMS_911_flag == 1)
				{
					for(int i=0; i<3; i++)
					LCD_command(0x4A, 0); //cursor right
					LCD_string("Sent");
				}

// 			while(RTC_raw_timer > (total_time - (total_time/4)*1) && RTC_raw_timer <= total_time && background_flag != 0)
// 			{
// 				LCD_command(0x46, 0); //cursor home
// 				for(int i=0; i<8; i++)
// 				LCD_command(0x4A, 0); //cursor right
// 				RTC_print_timer(RTC_raw_timer - ((total_time/4)*3));
// 			}
// 			
// 			
// 			while(RTC_raw_timer > (total_time - (total_time/4)*2) && RTC_raw_timer <= ((total_time - (total_time/4)*1)-1) && background_flag != 0)
// 			{
// 				LCD_command(0x45, 0x40); //second line
// 				for(int i=0; i<8; i++)
// 				LCD_command(0x4A, 0); //cursor right
// 				RTC_print_timer(RTC_raw_timer - ((total_time/4)*2));
// 			}
// 			
// 			
// 			while(RTC_raw_timer > (total_time - (total_time/4)*3) && RTC_raw_timer <= ((total_time - (total_time/4)*2)-1) && background_flag != 0)
// 			{
// 				LCD_command(0x45, 0x14); //third line
// 				for(int i=0; i<8; i++)
// 				LCD_command(0x4A, 0); //cursor right
// 				RTC_print_timer(RTC_raw_timer - ((total_time/4)*1));
// 			}
// 			
// 			
// 			while(RTC_raw_timer > 1 && RTC_raw_timer <= ((total_time - (total_time/4)*3)-1) && background_flag != 0)
// 			{
// 				LCD_command(0x45, 0x54); //fourth line
// 				for(int i=0; i<8; i++)
// 				LCD_command(0x4A, 0); //cursor right
// 				RTC_print_timer(RTC_raw_timer);
// 			}
			
// 			if(SMS_driver_flag == 1 && SMS_EC1_flag == 1 && SMS_EC2_flag == 1 && SMS_911_flag == 1 && background_flag != 0)
// 			{
// 				LCD_command(0x46, 0); //cursor home
// 				for(int i=0; i<8; i++)
// 				LCD_command(0x4A, 0); //cursor right
// 				LCD_string("00:00");
// 				for(int i=0; i<3; i++)
// 				LCD_command(0x4A, 0); //cursor right
// 				if(SMS_driver_flag == 1)
// 					LCD_string("Sent");
// 				
// 				LCD_command(0x45, 0x40); //second line
// 				for(int i=0; i<8; i++)
// 				LCD_command(0x4A, 0); //cursor right
// 				LCD_string("00:00");
// 				for(int i=0; i<3; i++)
// 				LCD_command(0x4A, 0); //cursor right
// 				if(SMS_EC1_flag == 1)
// 					LCD_string("Sent");
// 				
// 				LCD_command(0x45, 0x14); //third line
// 				for(int i=0; i<8; i++)
// 				LCD_command(0x4A, 0); //cursor right
// 				LCD_string("00:00");
// 				for(int i=0; i<3; i++)
// 				LCD_command(0x4A, 0); //cursor right
// 				if(SMS_EC2_flag == 1)
// 					LCD_string("Sent");
// 				
// 				LCD_command(0x45, 0x54); //fourth line
// 				for(int i=0; i<8; i++)
// 				LCD_command(0x4A, 0); //cursor right
// 				LCD_string("00:00");
// 				for(int i=0; i<3; i++)
// 				LCD_command(0x4A, 0); //cursor right
// 				if(SMS_911_flag == 1)
// 					LCD_string("Sent");
// 			}
			
		}
			
	}
	ESCAPE(); //escape once background flag is cleared
}

void UI_loading_animation(void)
{
	_delay_ms(500);
	if(menu_flag == 0)
		ESCAPE();
	for(int i=0; i<4; i++)
	{
		LCD_char('.');
		_delay_ms(500);
		if(menu_flag == 0)
			ESCAPE();
	}
	for(int i=0; i<4; i++)
		LCD_command(0x4E, 0); //backspace
}

void UI_store_number(volatile unsigned char* phone, volatile unsigned int starting_address)
{
	for(int i=0; i<11; i++)
	{
		if(i < 10)
			LCD_command(0x4B, 0); //blinking cursor
		else 
			LCD_command(0x4C, 0); //turn off blinking cursor
			
		keypad_read(); //read single character from keypad
		if((SPI_received_byte != 'A') && (SPI_received_byte != 'B') && (SPI_received_byte != 'C') && (SPI_received_byte != 'D') && (SPI_received_byte != '*') && (SPI_received_byte != '#') && (i < 10))
		{
			LCD_char(SPI_received_byte);
			phone[i] = SPI_received_byte;			
		}
		else if((SPI_received_byte == 'B') && (i != 0))
		{
			i -= 2;
			LCD_command(0x4E, 0); //backspace LCD
		}
		else if(i == 10)
		{
			if(SPI_received_byte == '#')
			{
				for(volatile unsigned int i = starting_address; i < (starting_address + 10); i++)
				{
					while(checkHIGH(EECR, EEPE)); //wait for EEPROM write enable to become 0
					EEAR = i; //EEPROM addresses 0-9 (out of 511)
					EEDR = phone[i - starting_address]; //EEPROM data
					EECR = 0b00000100; //Enable master. At same time disable write enable
					setHIGH(EECR, EEPE); //Enable EEPROM write enable
				}
			}
			else if(SPI_received_byte == 'D')
				ESCAPE();
			else
				i--;
		}
		else if(SPI_received_byte == 'D')
			ESCAPE();
		else
		{
			i--;
		}
	}
}

void UI_top_menu(void)
{
		LCD_command(0x51, 0); //clear LCD
		LCD_command(0x46, 0); //cursor home
		LCD_command(0x4C, 0); //turn off blinking cursor
		
		LCD_string("   Forget Me Not");
		LCD_command(0x45, 0x40); //second line
		LCD_string("(A) Status Info");
		LCD_command(0x45, 0x14); //third line
		LCD_string("(*) Set Up");
		LCD_command(0x45, 0x54); //fourth line
		LCD_string("(#) View #s");
}

void ESCAPE(void)
{
	setHIGH(EIMSK, INT2); //re-enable external interrupts on INT2
	menu_flag = 0;
	view_flag = 0;
	background_flag = 0;
	status_flag = 0;
	
	UART0_TX("$$$"); //enter command mode
	_delay_ms(100);
	UART0_TX("Q,2\r"); //discovery disabled
	_delay_ms(100);
	UART0_TX("---\r"); //exit command mode
	_delay_ms(100);
	
// 	if(BT_connection_flag == 0)
// 	{
// 		LCD_command(0x51, 0); //clear LCD
// 		LCD_command(0x4C, 0); //turn off blinking cursor
// 		LCD_command(0x46, 0); //cursor home
// 		LCD_string("Countdown commenced");
// 		LCD_command(0x45, 0x40); //second line
// 		for(int i=0; i<8; i++)
// 		LCD_command(0x4A, 0); //cursor right
// 		RTC_print_timer(RTC_raw_timer);
// 		LCD_command(0x45, 0x54); //fourth line
// 		LCD_string("    Press 'D' to abort");
// 	}
// 	else
	UI_top_menu();	
	main_processes();
}

void UI_copy_numbers(void)
{
	for(int i=0; i<10; i++)
	{
		while(checkHIGH(EECR, EEPE)); //wait for EEPROM write enable to become 0 (make sure a write operation is not occurring because you can't read and write simultaneously
		EEAR = i; //EEPROM addresses 0-9 (out of 511)
		setHIGH(EECR, EERE); //Read enable
		driver_phone[i] = EEDR;
	}
	
	for(int i=0; i<10; i++)
	{
		while(checkHIGH(EECR, EEPE)); //wait for EEPROM write enable to become 0 (make sure a write operation is not occurring because you can't read and write simultaneously
		EEAR = i+10; //EEPROM addresses 0-9 (out of 511)
		setHIGH(EECR, EERE); //Read enable
		emergency1_phone[i] = EEDR;
	}
	
	for(int i=0; i<10; i++)
	{
		while(checkHIGH(EECR, EEPE)); //wait for EEPROM write enable to become 0 (make sure a write operation is not occurring because you can't read and write simultaneously
		EEAR = i+20; //EEPROM addresses 0-9 (out of 511)
		setHIGH(EECR, EERE); //Read enable
		emergency2_phone[i] = EEDR;
	}
}

void UI_status(void)
{
	status_flag = 1;
	setHIGH(PORTD, PD4);
	UART1_TX("AT\r\n");
	_delay_ms(25);
	UART1_TX("AT+CMGF=1\r\n"); //Enable SMS - FONA
	_delay_ms(25);
	UART1_TX("AT+CBC\r\n"); //gets battery charge status
	_delay_ms(25);
	UART1_TX("AT+CSQ\r\n"); //gets cell signal strength
	_delay_ms(25);
	/*DISPLAY_STATUS();*/
	
	LCD_command(0x51, 0); //clear LCD
	LCD_command(0x4C, 0); //turn off blinking cursor
	LCD_command(0x46, 0); //cursor home
	
	LCD_string_P(PSTR("Battery: "));
	LCD_string(charge_status);
	LCD_string_P(PSTR("%"));
	
	LCD_command(0x45, 0x40); //second line
	LCD_string_P(PSTR("Signal: "));
	LCD_string(Signal_strength);
	LCD_string_P(PSTR(" dB"));
	
	LCD_command(0x45, 0x14); //third line
	LCD_string_P(PSTR("Temperature: "));
	temperature_print(temperature);
	
	_delay_ms(3500);
	charge_status_count = 0;
	BATTERY_COMMA_COUNT = 0;
	BATTERY_count = 0;
	SIGNAL_STRENGTH_COUNT = 0;
	Signal_Flag = 0;
	Signal_Strength_Tracker = 0;
	setLOW(PORTD, PD4);
	ESCAPE(); //escape after 3 seconds
}