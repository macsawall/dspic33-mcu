#ifndef FONA_H_
#define FONA_H_

#define F_CPU 1000000
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <avr/eeprom.h>
#include <avr/pgmspace.h>

extern volatile unsigned char latitude[11];
extern volatile unsigned char longitude[12];
extern volatile unsigned char message1[];
extern volatile unsigned char message2[];
extern volatile char latitude_count;
extern volatile char longitude_count;
extern volatile unsigned char GPS_grab;
// extern volatile unsigned char FONA_received_message[4];
// extern volatile unsigned char *FONA__incoming_message_compare;
// extern volatile unsigned char FONA_incoming_message_count;
extern volatile unsigned char FONA_count;
extern volatile unsigned char BATTERY_COMMA_COUNT;
extern volatile unsigned char BATTERY_count;
extern volatile unsigned char SIGNAL_STRENGTH_COMMA_COUNT;
extern volatile unsigned char SIGNAL_STRENGTH_COUNT;
extern volatile unsigned char Signal_strength[4];
extern volatile unsigned char Signal_Strength_Tracker;
extern volatile unsigned char Signal_Flag;
extern volatile unsigned char charge_status[4];
extern volatile unsigned char charge_status_count;
extern volatile unsigned char driver_phone_flag;

void send_text(volatile unsigned char recipient[]);
void send_text_2(volatile unsigned char recipient[], volatile unsigned char message[]);
// void WARN_OF_LOW_COVERAGE(void);
// void DISPLAY_BATTERY_STATUS(void);
//void DISPLAY_STATUS(void);

#endif /* FONA_H_ */