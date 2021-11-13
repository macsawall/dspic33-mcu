#ifndef ADC_H_
#define ADC_H_

#define F_CPU 1000000
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <avr/eeprom.h>
#include <avr/pgmspace.h>

void ADC_init(void);



#endif /* ADC_H_ */