#ifndef GLOBALDEFINITIONS_H_
#define GLOBALDEFINITIONS_H_

#define setLOW(REG, BIT) (REG = (REG & ~(1<<BIT))) //for use with DDRx and PORTx registers, and writing to other registers one bit at a time)
#define setHIGH(REG, BIT) (REG = (REG | (1<<BIT))) //for use with DDRx and PORTx registers, and writing to other registers one bit at a time)
#define toggle(REG, BIT) (REG = (REG ^ (1<<BIT))) //toggles a pin
#define checkHIGH(REG, BIT) (REG & (1<<BIT)) //for use with PINx registers and reading other registers one bit at a time (to check low, use ! before macro)



#endif /* GLOBALDEFINITIONS_H_ */