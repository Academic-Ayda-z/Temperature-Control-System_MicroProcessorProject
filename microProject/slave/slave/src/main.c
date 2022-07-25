#include <avr/io.h>
#include <avr/interrupt.h>
#include <Lcd.h>
#include <stdlib.h>
double dutyCycle = 0;
void cooler(int value,int duty){
    dutyCycle=duty;
    // OC0 Pin
    DDRB |= (1<<PORTB3);

    TCCR0 = 0x00;

    // Fast PWM
    TCCR0 |= (1<<WGM01) | (1<<WGM00);

    // Clear OC0 on compare match
    TCCR0 |= (1<<COM01);

    // 1/8*clock Prescaling
    TCCR0 |= (1<<CS01);

    // Duty cycle
    OCR0 = (dutyCycle/100)*255;

    TIMSK |= (1<<TOIE0);
}
int main() {
    DDRC = 0xFF;
    DDRB = (0<<DDB7) | (1<<DDB6) | (0<<DDB5) | (0<<DDB4);

    // SPI initialization
    // SPI Type: Slave
    // SPI Clock Rate: 8MHz / 128 = 62.5 kHz
    // SPI Clock Phase: Cycle Half
    // SPI Clock Polarity: Low
    // SPI Data Order: MSB First
    SPCR = (1<<SPE) | (0<<DORD) | (0<<MSTR) | (0<<CPOL) | (0<<CPHA) | (1<<SPR1) | (1<<SPR0);
    SPSR = (0<<SPI2X);
    PORTC=0x00; 
    int value=0;
    
    while(1) {
        SPDR = '0';

        while (((SPSR >> SPIF) & 1) == 0);
        value=(int)SPDR;

        while (((SPSR >> SPIF) & 1) == 0);
        value=(int)SPDR*256+value;


        cooler(value,0);
        PORTC=0;

        if(value>=52 && value<113){
            PORTC=2;
            dutyCycle=50+(value-52);
            if (dutyCycle>100)dutyCycle=100;
            
           OCR0 = (dutyCycle/100)*255;
            
                }
        if(value<=41 && value>=0){  
            PORTC=4;
            dutyCycle=100;
            OCR0 = (dutyCycle/100)*255;
                }
            
        if(value>=113 && value<=206){
            PORTC=0x01;
            _delay_ms(20);
            PORTC=0x00;
            _delay_ms(15);
        }
    }

    
}
