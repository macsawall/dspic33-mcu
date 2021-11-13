#include "GlobalDefinitions.h"
#include "timers.h"


void timer1_init(void)
{
	setLOW(TCCR1B, WGM13); //use timer1 as CTC (Clear timer on compare match)
	setHIGH(TCCR1B, WGM12);
	setLOW(TCCR1A, WGM11);
	setLOW(TCCR1A, WGM10);
	setHIGH(TCCR1B, CS12); //clock select is I/O clock with 256 prescaler
	setLOW(TCCR1B, CS11);
	setLOW(TCCR1B, CS10);
	TCNT1 = 0;
	OCR1A = 3904; //19531; //value needed for 5 seconds
	setHIGH(TIMSK1, OCIE1A); //enable timer1 Output Compare Match A interrupts
	//setLOW(TIMSK1, OCIE1A); //disable timer1 Output Compare Match A interrupts
}