/*
-Implementation of the UART1 module on the dspic33ev256gm102;
-main() initializes UART1 and then waits to receive a byte via UART1;
 Once a byte is received, UART1 transmits (echoes) the byte back to where
 it came from (PuTTY or wherever).
-Interrupts are implemented for RX and TX. The RX ISR is entered when
 U1RXREG receives a new byte from the receive shift register. The opposite
 is true for the TX ISR.
 
UART1 RX = pin RB15
UART1 TX = pin RB10
*/

#include <p33EV256GM102.h>
#include <xc.h>
#include <stdio.h> 

#pragma config ICS = PGD3
#pragma config FWDTEN = OFF

//#define BIT(x,n) (((x) >> (n)) & 1)
volatile unsigned char spi_received_byte; //garbage variable
volatile unsigned char uart_received_byte;
volatile unsigned char new_byte_received; //flag
volatile unsigned char tx_string[2]; //2 character string for compatability with uart_tx function. Last charcter is '\0'


void uart_init(void); //initializes UART1
void uart_tx(volatile unsigned char string[]); //write (transmit) strings to putty 
void uart_to_lcd(volatile unsigned char key); //echoes single characters back to SPI LCD
void __attribute__((__interrupt__, __auto_psv__)) _U1RXInterrupt(void);
void __attribute__((__interrupt__, __auto_psv__)) _U1TXInterrupt(void);

int main(void)
{
  uart_init();
  uart_tx("Hello");

  while(1)
  {
    //wait to receive byte, then echo it back and clear the flag so we don't continuously transmit the same data
    while(!new_byte_received);
    tx_string[0] = uart_received_byte;
    uart_tx(tx_string);
    new_byte_received = 0; 
  }
  
  return 0;
}

void uart_init(void)
{
  RPINR18bits.U1RXR = 0b0101111; //UART1 RX set to RPI47 pin RB15
  RPOR4bits.RP42R = 0b000001; //UART1 TX set to RP42 pin RB10
  TRISBbits.TRISB15 = 1; //pin RB15 set as input for UART1 RX
  TRISBbits.TRISB10 = 0; //pin RB10 set as output for UART1 TX
  U1MODEbits.UARTEN = 1; //enable UART1
  U1MODEbits.PDSEL = 0b00; //8 bit data, no parity
  U1MODEbits.STSEL = 0; //1 stop bit
  U1STAbits.UTXEN = 1; //transmit enabled
  //U1STAbits.UTXBF = 0/1; //TX busy flag 1=full
  //U1STAbits.URXDA = 0/1; // 1=RX buffer has data
  U1BRG = 23; //baud rate = 9600
  //U1RXREG is RX register
  //U1TXREG is TX register
  IFS0bits.U1RXIF = 0; //UART1 RX interrupt flag
  IEC0bits.U1RXIE = 1; //enable interrupt for UART1 RX
  IPC2bits.U1RXIP = 0b001; //priority for UART1 RX
  IFS0bits.U1TXIF = 0; //UART1 TX interrupt flag
  IEC0bits.U1TXIE = 1; //enable interrupt for UART1 TX
  IPC3bits.U1TXIP = 0b001; //priority for UART1 TX
}

void uart_tx(volatile unsigned char string[])
{
  int i = 0;
  while(string[i] != '\0') //loop though string until all characters written
  {
    while(U1STAbits.UTXBF); //TX busy flag 1=full
    U1TXREG = string[i];
    i++;
  }
}

void uart_to_lcd(volatile unsigned char key)
{       
  if(key == 0x7F) //backspace key
  {
    //blueLCD_command(0x4E, 0);
  }
  else
  {
    LATBbits.LATB5 = 0; //active low SS enabled
    while(SPI1STATbits.SPITBF == 1);
    SPI1BUF = key;
    while(SPI1STATbits.SPIRBF == 0);
    spi_received_byte = SPI1BUF;
    LATBbits.LATB5 = 1; //active low SS disabled
  }
}

void __attribute__((__interrupt__, __auto_psv__)) _U1RXInterrupt(void)
{
  IFS0bits.U1RXIF = 0; //IFS0<11>IEC0<11>IPC2<14:12>
  //while(U1STAbits.URXDA);
  new_byte_received = 1;
  uart_received_byte = U1RXREG; //store the received byte
}

void __attribute__((__interrupt__, __auto_psv__)) _U1TXInterrupt(void)
{
  IFS0bits.U1TXIF = 0; //IFS0<12>IEC0<12>IPC3<2:0>
}
