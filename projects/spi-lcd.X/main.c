#include <p33EV256GM102.h>
#include <xc.h>
#include <stdio.h> 

#pragma config ICS = PGD3
#pragma config FWDTEN = OFF

//#define BIT(x,n) (((x) >> (n)) & 1)
unsigned char spi_received_byte; //garbage variable
unsigned int lcd_character_count; //counter that keeps track of the characters printed to the 20x4 LCD screen (counts from 0 to 79)

void delay100ms(void);
void spiLCD_init(void);
void spiLCD_char(unsigned char); //write characters to LCD
void spiLCD_command(unsigned char, unsigned char);
void spiLCD_string(unsigned char*); //write strings to LCD

int main(void)
{
  TRISBbits.TRISB6 = 0;
  LATBbits.LATB6 = 0;
  delay100ms(); //gives LCD time to boot up
  spiLCD_init();
  spiLCD_command(0x51, 0); //clear
  spiLCD_command(0x46, 0); //cursor home (1st line)
  spiLCD_command(0x53, 0x02); //set brightness
  lcd_character_count = 0;
  spiLCD_string("Mackenzie Lee Sawall");
  
  while(1)
  {
      //
  }
  
  return 0;
}

void spiLCD_command(unsigned char command, unsigned char parameter)
{
  spiLCD_char(0xFE);
  spiLCD_char(command);
  
  if (parameter != 0) //byte3 is not needed on all LCD commands. Just use 0 if not needed
  {
    spiLCD_char(parameter);
  }
  
  if (command == 0x4E) //backspace command
  {
    if (lcd_character_count == 0)
    {
      lcd_character_count = 79;
      spiLCD_command(0x45, 0x67); //end of 4th line
    }
    else if (lcd_character_count == 20)
    {
      lcd_character_count--;
      spiLCD_command(0x45, 0x13); //end of 1st line
    }    
    else if (lcd_character_count == 40)
    {
      lcd_character_count--;
      spiLCD_command(0x45, 0x53); //end of 2nd line
    }     
    else if (lcd_character_count == 60)
    {
      lcd_character_count--;
      spiLCD_command(0x45, 0x27); //end of 3rd line
    }        
    else
    {
      lcd_character_count--;
    }
  }
}

void spiLCD_char(unsigned char byte)
{
    LATBbits.LATB5 = 0; //active low SS enabled
    while(SPI1STATbits.SPITBF == 1);
    SPI1BUF = byte;
    while(SPI1STATbits.SPIRBF == 0);
    spi_received_byte = SPI1BUF;
    LATBbits.LATB5 = 1; //active low SS disabled
    
	  lcd_character_count++;
  
    if(lcd_character_count == 20)
    {
      spiLCD_command(0x45, 0x40); //start of 2nd line
    }
    else if(lcd_character_count == 40)
    {
      spiLCD_command(0x45, 0x14); //start of 3rd line
    }
    else if(lcd_character_count == 60)
    {
      spiLCD_command(0x45, 0x54); //start of 4th line
    }
    else if(lcd_character_count == 80)
    {
      spiLCD_command(0x45, 0x00); //start of 1st line
      lcd_character_count = 0;
    }
}

void spiLCD_string(unsigned char* string)
{
  unsigned int i = 0;
  
  i = 0;
  while(string[i] != '\0') //loop though string until all characters written
  {
    spiLCD_char(string[i]);
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
  SPI1CON1bits.SMP = 1; //Input data is sampled at the end of data output time (don't think this matters since we are only sending data, not receiving)
  SPI1CON1bits.CKP = 1; //Idle state for clock is a high level (required by LCD module)
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

