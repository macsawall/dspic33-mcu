#include "GlobalDefinitions.h"
#include "LED.h"

#define F_CPU 1000000
#include <avr/io.h>
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <time.h>

volatile unsigned int color = 0; 

void red(void)
{
	OCR0A = 25; //turn on "perfect red"
	setHIGH(DDRB, PB3); //red
	setLOW(DDRD, PD6);
	setLOW(DDRD, PD7);
}

void blue(void)
{
	OCR2A = 25; //turn on "perfect blue"
	setLOW(DDRB, PB3);
	setLOW(DDRD, PD6);
	setHIGH(DDRD, PD7);
}

void green(void)
{
	OCR2B = 1; //turn on "perfect green"
	setLOW(DDRB, PB3);
	setHIGH(DDRD, PD6);
	setLOW(DDRD, PD7);
}

void yellow(void)
{
	OCR0A = 1; //turn on "perfect red"
	OCR2B = 13; //turn on "perfect green"
	setHIGH(DDRB, PB3); //red
	setHIGH(DDRD, PD6); //green
	setLOW(DDRD, PD7); //blue
}

void purple(void)
{
	OCR0A = 30; //turn on "perfect red"
	OCR2A = 40; //turn on "perfect blue"
	setHIGH(DDRB, PB3); //red
	setLOW(DDRD, PD6); //green
	setHIGH(DDRD, PD7); //blue	
}

void no_color(void)
{
	setLOW(DDRD, PD6);
	setLOW(DDRD, PD7);
	setLOW(DDRB, PB3);
}


void PWM_init(void)
 {
// 	setHIGH(DDRB, PB3); //OCOA
// 	setHIGH(DDRD, PD6); //OC2A
// 	setHIGH(DDRD, PD7); //OC2B
// 	
	setLOW(TCCR0B, WGM02); //CTC mode, top value set to OCRA, flag set on TOP
	setHIGH(TCCR0A, WGM01);
	setLOW(TCCR0A, WGM00);
	setLOW(TCCR0A, COM0A1); //toggle on compare match at TOP
	setHIGH(TCCR0A, COM0A0);
// 	setLOW(TCCR0B, CS02);
// 	setHIGH(TCCR0B, CS01);
// 	setLOW(TCCR0B, CS00);
	setHIGH(TCCR0B, CS02); // set prescaler to 1024
	setLOW(TCCR0B, CS01);
	setHIGH(TCCR0B, CS00);
	
	setLOW(TCCR2B, WGM22); //CTC, top value set to OCRA, flag set on TOP
	setHIGH(TCCR2A, WGM21);
	setLOW(TCCR2A, WGM20);
	setLOW(TCCR2A, COM2A1);
	setHIGH(TCCR2A, COM2A0);
	setLOW(TCCR2A, COM2B1);
	setHIGH(TCCR2A, COM2B0);
	setHIGH(TCCR2B, CS22); //1024
	setHIGH(TCCR2B, CS21);
	setHIGH(TCCR2B, CS20);
	//setHIGH(TIMSK2, TOIE2);
	
	OCR2B = 125; //B<A
	OCR2A = 125;
	OCR0A = 12;
	
	setHIGH(TIMSK0, OCIE0A);
	setHIGH(TIMSK2, OCIE2A);
	setHIGH(TIMSK2, OCIE2B);
	
}

ISR(TIMER0_COMPA_vect)
{
	setHIGH(TIFR0, OCF0A); //red 
	//TCNT0 = 0;
}

ISR(TIMER2_COMPA_vect)
{
	setHIGH(TIFR2, OCF2A); //blue
	//TCNT2 = 0;
}

ISR(TIMER2_COMPB_vect)
{
	setHIGH(TIFR2, OCF2B);	//green
	//TCNT2 = 0;
}

ISR(TIMER1_COMPB_vect)
{
	setHIGH(TIFR1, OCF1B);
	TCNT1 = 0;
}
