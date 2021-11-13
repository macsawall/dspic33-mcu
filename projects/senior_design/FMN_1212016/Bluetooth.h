#ifndef BLUETOOTH_H_
#define BLUETOOTH_H_

#define F_CPU 1000000
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <avr/eeprom.h>
#include <avr/pgmspace.h>

extern volatile unsigned char *BT_status_connected;
//extern volatile unsigned char BT_status_connected[9];
extern volatile unsigned char *BT_status_disconnected;
//extern volatile unsigned char BT_status_disconnected[12];
extern volatile unsigned char BT_status[11]; //BT_status string initialized with 0s
extern volatile unsigned char BT_status_count; //used in USART0_RX ISR
extern volatile unsigned int BT_connection_flag; //initialize flag to 0 for "not connected"


void Bluetooth_init(void);
void BT_connection_query(void);

#endif /* BLUETOOTH_H_ */