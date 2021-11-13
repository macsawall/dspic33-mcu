#include <p33EV256GM102.h>
#include <xc.h>
#include <stdio.h> 

#pragma config ICS = PGD3
#pragma config FWDTEN = OFF

//Fosc=7.37MHz
//Fcy=Fosc/2=3.685MHz (No changes to PLL)
#define Fcy 3685000u

void servo1_pos(volatile unsigned short stop_time);
void servo2_pos(volatile unsigned short stop_time);
void servo3_pos(volatile unsigned short stop_time);
void servo4_pos(volatile unsigned short stop_time);
void __attribute__((__interrupt__, __auto_psv__)) _T4Interrupt(void);
void __attribute__((__interrupt__, __auto_psv__)) _T3Interrupt(void);
void __attribute__((__interrupt__, __auto_psv__)) _AD1Interrupt(void);
void delay(void);
void adc_init(void);
void initialize(void);

volatile unsigned num = 2000;
volatile unsigned short servo1[2000];
volatile unsigned short servo2[2000];
volatile unsigned short servo3[2000];
volatile unsigned short servo4[2000];
unsigned j = 0;

int main(void)
{
    //set up pins, timers, ADC, etc.
    initialize();
    
    //Fill arrays with safe values
    for(j=0; j<2000; j++)
    {
        servo1[j] = 1500;
        servo2[j] = 1500;
        servo3[j] = 1500;
        servo4[j] = 1500;
    }
    
    j = 0;//reset j to 0
    
    while(1)
    {
        //wait until button is pressed
        while(PORTAbits.RA2 == 1); 
        delay();
        delay();
        LATBbits.LATB6 = 1; //turn on red LED during recording
        LATBbits.LATB8 = 0; //turn off green LED during recording
        j = 0;
        //once button is pressed, record movement
        T2CONbits.TON = 0;
        T4CONbits.TON = 0; // Disable Timer 4
        T3CONbits.TON = 1; //enable timer 3
        IEC0bits.AD1IE = 1; 
        AD1CON1bits.ADON = 1; //enables ADC
        while(AD1CON1bits.ADON == 1);
        //wait until button is pressed again to play movement infinitely
        while(PORTAbits.RA2 == 1); 
        delay();
        delay();
        LATBbits.LATB6 = 0; //turn off red LED during playback
        LATBbits.LATB8 = 1; //turn on green LED during playback
        j = 0;
        //IEC0bits.AD1IE = 0; 
        //AD1CON1bits.ADON = 0; //disables ADC
        T2CONbits.TON = 1;
        T4CONbits.TON = 1;          // Enable Timer 4
        
        while(PORTAbits.RA2 == 1);
        delay();
        delay();
        LATBbits.LATB8 = 0; //turn off green LED
        T2CONbits.TON = 0;
        T4CONbits.TON = 0;          // Disable Timer 4
    }
    return 0;
}

void initialize(void)
{
    //setup LEDs and button
    TRISBbits.TRISB6 = 0; //red LED output
    ANSELBbits.ANSB8 = 0; //B8 to digital
    TRISBbits.TRISB8 = 0; //green LED output
    ANSELAbits.ANSA2 = 0;  //A2 to digital
    TRISAbits.TRISA2 = 1; //button input
    
    //setup OC1
    RPOR0bits.RP35R = 0b010000; //remap pin RP35 as OC1
    ANSELBbits.ANSB3 = 0; //sets RB3 to digital
    TRISBbits.TRISB3 = 0; //RB3 output for OC1
    OC1R = 0; // pulse start time
    OC1CON1bits.OCM = 0b101; // double compare continuous pulse mode
    
    //setup OC2
    RPOR1bits.RP37R = 0b010001; //remap pin RP37 as OC2
    TRISBbits.TRISB5 = 0; //RB5 output for OC2
    OC2R = 0; // pulse start time
    OC2CON1bits.OCM = 0b101; // double compare continuous pulse mode
    
    //setup OC3
    RPOR1bits.RP36R = 0b010010; //remap pin RP36 as OC3
    TRISBbits.TRISB4 = 0; //RB4 output for OC3
    OC3R = 0; // pulse start time
    OC3CON1bits.OCM = 0b101; // double compare continuous pulse mode
    
    //setup OC4
    RPOR3bits.RP41R = 0b010011; //remap pin RP41 as OC4
    ANSELBbits.ANSB9 = 0; //sets RB9 to digital
    TRISBbits.TRISB9 = 0; //RB9 output for OC4
    OC4R = 0; // pulse start time
    OC4CON1bits.OCM = 0b101; // double compare continuous pulse mode
    
    // Configure Timer 2 for controlling 20 ms period of OC1-OC4
    PR2 = 9213;             // Timer 2 period (20ms) Fcy x 20ms / 8
    T2CONbits.TCKPS = 0b01; // Timer 2 prescaler 1:8
    //T2CONbits.TON = 1;      // Enable Timer 2
    
    // Configure Timer 4 for controlling playback
    IPC6bits.T4IP = 3;
    IFS1bits.T4IF = 0;
    IEC1bits.T4IE = 1; 
    PR4 = 27637;// 15sample at 1/0.00052=1919 samples/s   //28789; // Timer 4 period (2s) Fcy x 2s / 256
    T4CONbits.TCKPS = 0b00;     // Timer 4 prescaler 1:1
    T4CONbits.TON = 0;          // Disable Timer 4
    
    // Configure Timer 3 for controlling ADC sampling/conversion
    IPC2bits.T3IP = 2;
    IFS0bits.T3IF = 0;
    IEC0bits.T3IE = 1; 
    PR3 = 13819;// 15 sample at 1/= 750 samples/s for 10 second duration  //28789; // Timer 3 period (2s) Fcy x 2s / 
    T3CONbits.TCKPS = 0b00;     // Timer 3 prescaler 1:1
    T3CONbits.TON = 0;          // Disable Timer 3
    
    adc_init();
}

void delay(void)
{
    unsigned i=0;
    for(i=0; i<65000; i++)
    {
        //
    }
}

void adc_init(void)
{
    IPC3bits.AD1IP = 1;
    IFS0bits.AD1IF = 0;
    IEC0bits.AD1IE = 0; 
	TRISBbits.TRISB2 = 1; //input for ADC channel 0
    ANSELBbits.ANSB2 = 1; //B2 to analog
    TRISBbits.TRISB1 = 1; //input for ADC channel 1
    ANSELBbits.ANSB1 = 1; //B1 to analog
    TRISAbits.TRISA0 = 1; //input for ADC channel 2
    ANSELAbits.ANSA0 = 1; //A0 to analog
    TRISBbits.TRISB7 = 1; //input for ADC channel 3
    ANSELBbits.ANSB7 = 1; //B7 to analog
    AD1CON1bits.AD12B = 0; //10 bit, 4 channel operation
    AD1CON1bits.SIMSAM = 0; //samples channels sequentially
    AD1CON1bits.SSRC = 0b010; //Timer 3 compare ends sampling and starts conversion
    AD1CON1bits.ASAM = 1; //Sampling begins immediately after last conversion; SAMP bit is auto-set
    AD1CON2bits.CHPS = 0b10; //converts all channels
    AD1CON2bits.SMPI = 0b0100; //interrupt is called after every 4th sample/conversion
    AD1CON3bits.ADRC = 0; //clock derived from system clock WAS 1 BEFORE FOR INTERNAL ADC RC CLOCK!!!
    AD1CON3bits.SAMC = 0b00000;
    AD1CON3bits.ADCS = 0x00;
    AD1CHS0bits.CH0SB = 0b000100; // channel 0 positive input is AN4 for mux B
    AD1CHS0bits.CH0SA = 0b000100; // channel 0 positive input is AN4 for mux A
    
    AD1CHS123bits.CH123SB2 = 0; //CH1 positive input is AN3, CH2 positive input is AN0, CH3 positive input is AN25 (mux B)
    AD1CHS123bits.CH123SB1 = 1;
    AD1CHS123bits.CH123SB0 = 1;
    
    AD1CHS123bits.CH123SA2 = 0; //CH1 positive input is AN3, CH2 positive input is AN0, CH3 positive input is AN25 (mux A)
    AD1CHS123bits.CH123SA1 = 1;
    AD1CHS123bits.CH123SA0 = 1;
    
    AD1CON1bits.ADON = 0; //disables ADC
}

void servo1_pos(volatile unsigned short stop_time) //stop time in microseconds
{
    // Configure Output Compare channel 1 (OC1)
    OC1RS = (volatile unsigned short)(((Fcy/8)*stop_time)/1000000); // pulse stop time (1.5ms) OC1RS = Fcy x 1.5ms / 8
}

void servo2_pos(volatile unsigned short stop_time) //stop time in microseconds
{
    // Configure Output Compare channel 1 (OC1)
    OC2RS = (volatile unsigned short)(((Fcy/8)*stop_time)/1000000); // pulse stop time (1.5ms) OC1RS = Fcy x 1.5ms / 8
}

void servo3_pos(volatile unsigned short stop_time) //stop time in microseconds
{
    // Configure Output Compare channel 1 (OC1)
    OC3RS = (volatile unsigned short)(((Fcy/8)*stop_time)/1000000); // pulse stop time (1.5ms) OC1RS = Fcy x 1.5ms / 8
}

void servo4_pos(volatile unsigned short stop_time) //stop time in microseconds
{
    // Configure Output Compare channel 1 (OC1)
    OC4RS = (volatile unsigned short)(((Fcy/8)*stop_time)/1000000); // pulse stop time (1.5ms) OC1RS = Fcy x 1.5ms / 8
}

void __attribute__((__interrupt__, __auto_psv__)) _T4Interrupt(void)
{
    // Clear Timer 4 interrupt flag
    IFS1bits.T4IF = 0;
    // Toggle LED on RD1
    //LATBbits.LATB6 = ~LATBbits.LATB6;
    if(j<2000)
    {
        servo1_pos(servo1[j]);
        servo2_pos(servo2[j]);
        servo3_pos(servo3[j]);
        servo4_pos(servo4[j]);
        j++;
    }
    if(j==2000)
        j = 0;
}

void __attribute__((__interrupt__, __auto_psv__)) _T3Interrupt(void)
{
    // Clear Timer 3 interrupt flag
    IFS0bits.T3IF = 0;
}

//ISR called after every 4th conversion is complete
void __attribute__((__interrupt__, __auto_psv__)) _AD1Interrupt(void)
{    
    IFS0bits.AD1IF = 0;
    if(j < 2000)
    {
        servo1[j] = (unsigned short)(((((2450L-625)*ADC1BUF0)/(373)) + 214)); //calculates servo position between 2500 and 550
        //servo2[j] = (unsigned short)(((((2200L-540)*ADC1BUF1)/(338)) + 250)); //calculates servo position between 2500 and 550
        servo2[j] = (unsigned short)(((((2500L-550)*ADC1BUF1)/(358)) + 114));
        servo3[j] = (unsigned short)(((((2500L-550)*ADC1BUF2)/(389)) + 234)); //calculates servo position between 2500 and 550
        servo4[j] = (unsigned short)(((((2420L-560)*ADC1BUF3)/(383)) + 230)); //calculates servo position between 2500 and 550
        j++;
    }
    if(j == 2000)
    {
        IEC0bits.AD1IE = 0; 
        AD1CON1bits.ADON = 0; //disables ADC
        T3CONbits.TON = 0; //disable timer 3
        LATBbits.LATB6 = 0; //turn off red LED once recording is done
    }
    
}