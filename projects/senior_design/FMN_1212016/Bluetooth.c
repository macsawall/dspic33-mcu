#include "Bluetooth.h"
#include "GlobalDefinitions.h"
#include "UART.h"
#include "RTC.h"
#include "UI.h"
#include "main.h"
#include "FONA.h"
#include "LED.h"

volatile unsigned char *BT_status_connected = "iCONNECT";
//volatile unsigned char BT_status_connected[9] = "iCONNECT";
volatile unsigned char *BT_status_disconnected = "iDISCONNECT";
//volatile unsigned char BT_status_disconnected[12] = "iDISCONNECT";
volatile unsigned char BT_status[11] = {'0'}; //BT_status string initialized with 0s. Big enough to hold "iDISCONNECT"
volatile unsigned char BT_status_count = 0; //used in USART0_RX ISR
volatile unsigned int BT_connection_flag = 2; //initialize flag to 0 for "never connected"

void Bluetooth_init(void)
{
	UART0_TX("$$$"); //enter command mode on RN41
	_delay_ms(100);
	//SET commands
		UART0_TX("SF,1\r"); //factory reset
		_delay_ms(100);
	UART0_TX("SA,1\r"); //SSP keyboard I/O mode (default)
	_delay_ms(100);
	UART0_TX("SI,0500\r"); //inquiry scan window (discoverability). Default = 0x0100
	_delay_ms(100);
	UART0_TX("SJ,0500\r"); //page scan window (connectibility). Default = 0x0100
	_delay_ms(100);
	UART0_TX("SL,N\r"); //parity set to none
	_delay_ms(100);
	UART0_TX("SM,6\r"); //pairing mode
	_delay_ms(100);
	UART0_TX("S~,6\r"); //HID profile (needed for iPhone discoverability)
	_delay_ms(100);
	UART0_TX("SN,ForgetMeNot\r"); //sets device name
	_delay_ms(100);
	UART0_TX("SO,i\r"); //sets extended status string so we can poll for a disconnect
	_delay_ms(100);
	UART0_TX("SP,1234\r"); //set pin code to 1234 (default)
	_delay_ms(100);
	UART0_TX("SQ,8\r"); //default special configuration (disable discovery at startup!)
	_delay_ms(100);
	UART0_TX("SU,96\r"); //set the baud rate to 9600bps
	_delay_ms(100);
	//UART_TX("SW,0050\n"); //enables Sniff Mode with interval time of 50ms (we may want this, maybe not)
	UART0_TX("SY,0010\r"); //set the transmit power to max power (we will reduce this later)
	_delay_ms(100);
	UART0_TX("Q\r"); //quiet mode (non-discoverable)
	_delay_ms(100);
	UART0_TX("SX,1\r"); //bonding enabled
	_delay_ms(100);
	UART0_TX("R,1\r"); //reboot RN41 so settings take effect
	_delay_ms(1000);
	//SO,<string> is what we will poll for any disconnection from phone
}

void BT_connection_query(void)
{
	volatile unsigned int connected_count = 0;
	volatile unsigned int disconnected_count = 0;

	for(int i=0; i<11; i++)
	{	
			if(BT_status[i] == *(BT_status_connected + i)) //if the current char says "iCONNECT"
			{
				connected_count++;
			}
			if(BT_status[i] == *(BT_status_disconnected + i)) //if the current char says "iDISCONNECT"
			{
				disconnected_count++;
			}
	}

	if(connected_count >= 7) //if iCONNECT
	{
		BT_connection_flag = 1;
		setLOW(PORTD, PD4); //turn off Fona Reference Voltage
		asm volatile("Nop");
		RTC_write(0x11, 0); //load timer with 0 to stop it
		setLOW(TIMSK1, OCIE1A); //disable timer1 Output Compare Match A interrupts
		RTC_raw_timer = total_time;
		FONA_FLAG = 0; 
		SMS_driver_flag = 0;
		SMS_EC1_flag = 0;
		SMS_EC2_flag = 0;
		SMS_911_flag = 0;
		latitude_count = 0;
		longitude_count = 0;
		GPS_comma_count = 0;
		GPS_grab = 0;
		FONA_count = 0;
		no_color();
		for(int i=0; i<11; i++) //reset BT_status string for next time
			BT_status[i] = '0';
	}
	if(disconnected_count >= 10) //if iDISCONNECT
	{
		BT_connection_flag = 0;
		setHIGH(PORTD, PD4); //turn on Fona Reference Voltage
		asm volatile("Nop");
		UART1_TX("AT\r\n");
		_delay_ms(25);
		UART1_TX("AT+CMGD=30,4\r\n"); //delete all sms messages
		_delay_ms(25);
// 		UART1_TX("AT+CGNSPWR=0\r\n");
// 		_delay_ms(25);
		UART1_TX("AT+CGNSPWR=1\r\n"); //Enable GPS - FONA
		_delay_ms(25);
		RTC_write(0x11, total_time); //load timer with set time
		RTC_raw_timer = total_time;
		setHIGH(TIMSK1, OCIE1A); //enable timer1 Output Compare Match A interrupts
		UART0_TX("$$$"); //enter command mode
		_delay_ms(100);
		UART0_TX("Q,2\r"); //discovery disabled
		_delay_ms(100);
		UART0_TX("---\r"); //exit command mode
		_delay_ms(100);
		
		for(int i=0; i<11; i++) //reset BT_status string for next time
			BT_status[i] = '0';
			
		UART1_TX("AT+CGNSINF\r\n"); //grab GPS coordinates
		_delay_ms(25);
	}
}