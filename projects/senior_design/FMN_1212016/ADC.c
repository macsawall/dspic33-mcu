#include "GlobalDefinitions.h"
#include "ADC.h"

void ADC_init(void)
{
	setLOW(DDRA, PA3); //set channel 3 to input
	setLOW(ADMUX, REFS1); //reference voltage set to AREF
	setLOW(ADMUX, REFS0);
	setLOW(ADMUX, MUX4); //Channel 3 select
	setLOW(ADMUX, MUX3);
	setLOW(ADMUX, MUX2);
	setHIGH(ADMUX, MUX1);
	setHIGH(ADMUX, MUX0);
	//setHIGH(ADCSRA, ADIE); //enable interrupts
// 	setHIGH(ADCSRA, ADPS2); //clock division /128
// 	setHIGH(ADCSRA, ADPS1);
// 	setHIGH(ADCSRA, ADPS0);
	setLOW(ADCSRA, ADPS2); //clock division /128
	setLOW(ADCSRA, ADPS1);
	setLOW(ADCSRA, ADPS0);
	// 	setLOW(ADCSRA, ADPS2); //clock division /128
	// 	setLOW(ADCSRA, ADPS1);
	// 	setLOW(ADCSRA, ADPS0);
	/*setHIGH(ADCSRA, ADATE); //auto trigger enable*/
	setLOW(ADCSRA, ADATE); //auto trigger disabled
	//setLOW(ADCSRB, ADTS2); //free running mode
	//setLOW(ADCSRB, ADTS1);
	//setLOW(ADCSRB, ADTS0);
	setHIGH(ADCSRA, ADEN); //enable ADC
	setHIGH(ADCSRA, ADSC); //start conversion
}