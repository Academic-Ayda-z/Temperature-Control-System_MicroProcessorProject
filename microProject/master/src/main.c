#include <avr/io.h>
#include <avr/interrupt.h>
#include <Lcd.h>
#include <stdlib.h>
//portc insted of portb in lcd.c
unsigned char i,microlearn[20];
char ignore;
void show_LCD(){
    DDRC = 0xFF;
    DDRD=0x07; 
      
    init_LCD();
    
    LCD_cmd(0x0F); // Make blinking cursor
    _delay_ms(1000);
    
    long int value=(int)ADCL + (int)ADCH*256;
    for (int i=0;i<4;i++){
    microlearn[i]= (int)value%10+'0';//(unsigned char)ADCW+'0';			
    value/=10;
   }
    for(int i=4;i>-1;i--) {
    LCD_write(microlearn[i]); 
    
    }
    _delay_ms(3000);
    LCD_cmd(0x01);  // clear screen
    LCD_cmd(0x14);
}
void ACD_A(){
    // ADC initialization
    ADMUX = (0 << REFS1) | (1 << REFS0); // AVCC
    ADCSRA = (1<<ADEN) | (0<<ADPS2) | (1<<ADPS1) | (1<<ADPS0);  // prescale = 8
    _delay_ms(1000);

    ADCSRA |= ((1 << ADSC) | (1 << ADIF)); // start conversion
    while( (ADCSRA & (1 << ADIF)) == 0 ); // wait till end of the conversion
    show_LCD();
}
void send_A(){
    DDRB = (1<<DDB7) | (0<<DDB6) | (1<<DDB5) | (1<<DDB4) | (1<<DDB3);
    PORTB = (1<<PORTB4);// | (1<<PORTB3); // slave sel

    // SPI initialization
    // SPI Type: Master
    // SPI Clock Rate: 8MHz / 128 = 62.5 kHz
    // SPI Clock Phase: Cycle Half
    // SPI Clock Polarity: Low
    // SPI Data Order: MSB First
    SPCR = (1<<SPE) | (0<<DORD) | (1<<MSTR) | (0<<CPOL) | (0<<CPHA) | (1<<SPR1) | (1<<SPR0);
    SPSR = (0<<SPI2X);
    
    PORTB &= ~(1<<PORTB4); // Select Slave #1
    SPDR =(unsigned char) ADCW;//ADCL; // send A
    while(((SPSR >> SPIF) & 1) == 0);
    ignore = SPDR;
//    PORTB |= (1<<PORTB4); // Deselect Slave #1

  //  PORTB &= ~(1<<PORTB4); // Select Slave #1    
    SPDR = (ADCW>>8);//ADCH;
    while(((SPSR >> SPIF) & 1) == 0);
    ignore = SPDR;
    PORTB |= (1<<PORTB4); // Deselect Slave #1
    
}
int main(void){

    SFIOR |= (0<<ACME); // disable ADC multiplexer output
    ACSR  |= (0<<ACD) | (0<<ACBG) | (0<<ACO) | (0<<ACI) | (0<<ACIE) | (0<<ACIC) | (0<<ACIS1) | (0<<ACIS0);

    while(1){
        if(((ACSR >> ACO) & 1) == 1){
            ACD_A();
            send_A();
        }
        else{
            PORTB = (0<<PORTB4);
        }
    }
}
