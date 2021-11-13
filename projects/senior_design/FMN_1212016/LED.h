#ifndef LED_H_
#define LED_H_

#define F_CPU 1000000
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <avr/eeprom.h>
#include <avr/pgmspace.h>

extern volatile unsigned int color;
void PWM_init(void);
void red(void);
void blue(void);
void green(void);
void yellow(void);
void purple(void);
void no_color(void);


#endif /* LED_H_ */