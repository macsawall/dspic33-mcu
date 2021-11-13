#ifndef UI_H_
#define UI_H_

#define F_CPU 1000000
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <avr/eeprom.h>
#include <avr/pgmspace.h>

#define total_time 61

extern volatile unsigned char SPI_received_byte;
extern volatile unsigned char driver_phone[11];
extern volatile unsigned char emergency1_phone[11];
extern volatile unsigned char emergency2_phone[11];
extern volatile unsigned int menu_flag;
extern volatile unsigned int BT_discovery_flag;
extern volatile unsigned int view_flag;
extern volatile unsigned int background_flag;
extern volatile unsigned int status_flag;
extern volatile unsigned char toggle_911;

extern volatile unsigned int SMS_driver_flag;
extern volatile unsigned int SMS_EC1_flag;
extern volatile unsigned int SMS_EC2_flag;
extern volatile unsigned int SMS_911_flag;

void UI_top_menu(void);
void ESCAPE(void);
void UI_menu(void);
void UI_view(void);
void UI_background(void);
void UI_status(void);
void UI_loading_animation(void);
void UI_store_number(volatile unsigned char* phone, volatile unsigned int starting_address);
void UI_copy_numbers(void);


#endif /* UI_H_ */