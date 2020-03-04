#include <p33EV256GM102.h>
#include <xc.h>
#include <stdio.h> 

#pragma config ICS = PGD3
#pragma config FWDTEN = OFF

//#define BIT(x,n) (((x) >> (n)) & 1)
volatile unsigned char SPI_received_byte; //garbage variable

void delay100ms(void);
void spiLCD_init(void);
void spiLCD_command(char, char);
void spiLCD_write(volatile unsigned char string[]); //write strings to LCD
void UARTtoLCD(volatile unsigned char key); //echoes single characters back to putty and LCD
//void __attribute__((__interrupt__, __auto_psv__)) _U1RXInterrupt(void);
//void __attribute__((__interrupt__, __auto_psv__)) _U1TXInterrupt(void);


int main(void)
{
  TRISBbits.TRISB6 = 0;
  LATBbits.LATB6 = 0;
  delay100ms(); //gives LCD time to boot up
  spiLCD_init();
  spiLCD_command(0x51, 0); //clear
  spiLCD_command(0x46, 0); //cursor home (1st line)
  spiLCD_command(0x53, 0x02); //set brightness
  spiLCD_write("1st");
  spiLCD_command(0x45, 0x40); //2nd line
  spiLCD_write("2nd");
  spiLCD_command(0x45, 0x14); //3rd line
  spiLCD_write("3rd");
  spiLCD_command(0x45, 0x54); //4th line
  spiLCD_write("4th");
  while(1)
  {
      //
  }
  
  return 0;
}

void spiLCD_command(char byte2, char byte3)
{
  LATBbits.LATB5 = 0; //active low SS enabled
  while(SPI1STATbits.SPITBF == 1);
  SPI1BUF = 0xFE;
  while(SPI1STATbits.SPIRBF == 0);
  SPI_received_byte = SPI1BUF;
  
  while(SPI1STATbits.SPITBF == 1);
  SPI1BUF = byte2;
  while(SPI1STATbits.SPIRBF == 0);
  SPI_received_byte = SPI1BUF;
  //SPI1STATbits.SPIROV = 0;
  
  if (byte3 != 0)
  {
    while(SPI1STATbits.SPITBF == 1);
    SPI1BUF = byte3;
    while(SPI1STATbits.SPIRBF == 0);
    SPI_received_byte = SPI1BUF;
    //SPI1STATbits.SPIROV = 0;
    LATBbits.LATB5 = 1; //active low SS disabled
  }
  
  LATBbits.LATB5 = 1; //active low SS disabled
}

void spiLCD_write(volatile unsigned char string[])
{
  int i = 0;
  while(string[i] != '\0') //loop though string until all characters written
  {
     LATBbits.LATB5 = 0; //active low SS enabled
     while(SPI1STATbits.SPITBF == 1);
     SPI1BUF = string[i];
     while(SPI1STATbits.SPIRBF == 0);
     SPI_received_byte = SPI1BUF;
     LATBbits.LATB5 = 1; //active low SS disabled
     i++;
  }
}

void spiLCD_init(void)
{
  ANSELBbits.ANSB8 = 0; //sets rb8 to digital
  ANSELBbits.ANSB7 = 0; //sets rb7 to digital
  TRISBbits.TRISB8 = 0; //SDO output
  TRISBbits.TRISB7 = 0; //SCK output
  TRISBbits.TRISB5 = 0; //SS output
  LATBbits.LATB5 = 1; //active low SS disabled
  SPI1CON1bits.MSTEN = 1; // Master mode
  SPI1CON1bits.SMP = 1;
  SPI1CON1bits.CKP = 1;
  SPI1STATbits.SPIEN = 1; //enables SPI
}

void delay100ms(void)
{
  long i = 0;
  long delay = 100000;
  //long delay = 368500;
  for(i=0; i<delay; i++)
  {
  	Nop();
  }
}

void UARTtoLCD(volatile unsigned char key)
{
  while(U1STAbits.UTXBF);
  U1TXREG = key;   
  
  if(key == 0x7F) //backspace key
  {
    spiLCD_command(0x4E, 0);
  }
  else
  {
    LATBbits.LATB5 = 0; //active low SS enabled
    while(SPI1STATbits.SPITBF == 1);
    SPI1BUF = key;
    while(SPI1STATbits.SPIRBF == 0);
    SPI_received_byte = SPI1BUF;
    LATBbits.LATB5 = 1; //active low SS disabled
  }
}

/*
void __attribute__((__interrupt__, __auto_psv__)) _U1RXInterrupt(void)
{
  IFS0bits.U1RXIF = 0; //IFS0<11>IEC0<11>IPC2<14:12>
  //while(U1STAbits.URXDA);
  UART_received_byte = U1RXREG;
  //bluetype(UART_received_byte);
  //UART_TX(UART_received_byte);
  UARTtoLCD(UART_received_byte);
  LATBbits.LATB6 = ~LATBbits.LATB6;
}

void __attribute__((__interrupt__, __auto_psv__)) _U1TXInterrupt(void)
{
  IFS0bits.U1TXIF = 0; //IFS0<12>IEC0<12>IPC3<2:0>
}
*/