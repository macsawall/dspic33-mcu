#include "GlobalDefinitions.h"
#include "I2C.h"

void I2C_init(void)
{
	TWBR = 0x0C; //bit rate set to 25kHz (Fcpu = 1MHz) with prescaler = 1
	setHIGH(TWCR, TWEN); //enables I2C
}