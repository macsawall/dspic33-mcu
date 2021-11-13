#include "GlobalDefinitions.h"
#include "DAC.h"

int sinewave;

const int sine_table[200] PROGMEM = {512,528,544,560,576,592,607,623,639,654,670,685,700,715,729,744,758,772,786,799,
	812,825,838,850,862,873,884,895,906,916,925,935,943,952,960,967,974,981,987,993,
	998,1003,1007,1011,1014,1017,1019,1021,1022,1023,1023,1023,1022,1021,1019,1017,1014,1011,1007,1003,
	998,993,987,981,974,967,960,952,943,935,925,916,906,895,884,873,862,850,838,825,
	812,799,786,772,758,744,729,715,700,685,670,654,639,623,607,592,576,560,544,528,
	512,495,479,463,447,431,416,400,384,369,353,338,323,308,294,279,265,251,237,224,
	211,198,185,173,161,150,139,128,117,107,98,88,80,71,63,56,49,42,36,30,
	25,20,16,12,9,6,4,2,1,0,0,0,1,2,4,6,9,12,16,20,
	25,30,36,42,49,56,63,71,80,88,98,107,117,128,139,150,161,173,185,198,
211,224,237,251,265,279,294,308,323,338,353,369,384,400,416,431,447,463,479,495};


void sinewave_out(void)
{
	int lowbyte;
	int highbyte;
	int j = 0;
	for(j = 0; j < 200; j++)
	{
		sinewave = sine_table[j]; //assign variable sinewave to single sample from sine_table
		sinewave = (sinewave << 2);//shift sinewave left twice because bits 0 & 1 are don't cares
		sinewave = (0xF000 | sinewave);//append DAC code to front of sinewave (now at 16 bits)
		lowbyte = (sinewave & 0x00FF);//split sinewave into low byte...
		highbyte = ((sinewave & 0xFF00) >> 8);//...and high byte
		
		setLOW(PORTB, PB1); //CS low = enables SPI
		SPDR = highbyte;
		while(!checkHIGH(SPSR, SPIF)); //wait until transfer complete
		SPDR = lowbyte;
		while(!checkHIGH(SPSR, SPIF)); //wait until transfer complete
		setHIGH(PORTB, PB1); //CS high = disables SPI
		custom_delay(j);
		for(unsigned int k=0; k<13 ;k++)
		{
			custom_delay(k);
		}
	}

}

void custom_delay(int delay)
{
	while(delay > 0)
	{
		asm volatile ("Nop");
		delay--;
	}
}