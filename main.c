#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <stdio.h>
#include <stdbool.h>
#include "uart.h"
#include "millis.h"

// https://wokwi.com/projects/365047092561218561

// B (digital pin 8 to 13)
// C (analog input pins)
// D (digital pins 0 to 7)
#define LED_PIN 2
#define BUTTON_PIN 1

volatile int changes = 0;

#define BIT_SET(a, b) ((a) |= (1ULL << (b)))
#define BIT_CLEAR(a,b) ((a) &= ~(1ULL<<(b)))
#define BIT_FLIP(a,b) ((a) ^= (1ULL<<(b)))
#define BIT_CHECK(a,b) (!!((a) & (1ULL<<(b)))) 

volatile unsigned char buttonTickResult_PB1 = 0;

typedef enum{
    Click_Status_Pressed,
    Click_Status_Relesed,
    Click_Status_Notsure
}Click_Status;

Click_Status Clicked(unsigned char ch){
    unsigned char mask = 0b00011111;
    if((ch & mask) == 0b00011111) return Click_Status_Pressed;
    if((ch & mask) == 0) return Click_Status_Relesed;
    return Click_Status_Notsure;
}


#define BUTTON_IS_CLICKED(PINB, BUTTON_PIN) !BIT_CHECK(PINB, BUTTON_PIN)

volatile unsigned int t = 0;
volatile int n = 0;

ISR(TIMER2_OVF_vect)
{
    TCNT2 = 5;
    bool sample = BUTTON_IS_CLICKED(PINB, BUTTON_PIN);
    buttonTickResult_PB1 = buttonTickResult_PB1 << 1;
    buttonTickResult_PB1 |= sample;    
}

void timer0_init()
{
    TCCR2A = 0;
    TCCR2B = 0;
    TCCR2B |= 0b00000111;
    TCNT2 = 5;
    TIMSK2 |= 0b00000001;

}

int main(void){
    init_serial();
    millis_init();
    sei();
    BIT_SET(DDRB, LED_PIN); //OUTPUT MODE
    //Sätt till INPUT_PULLUP
    BIT_CLEAR(DDRB, BUTTON_PIN); // INPUT MODE
    BIT_SET(PORTB, BUTTON_PIN); 
    // DATA DIRECTION REGISTER = avgör mode
    // om output så skickar vi  1 eller 0 på motsvarande pinne på PORT
    // om input så läser vi  1 eller 0 på motsvarande pinne på PIN
    bool isOn = true;
    bool canRelese = false;
    int old, changes;
    while(1){
        // Om kanppen är tryct så LYS
        // annars lys inte
        // PIND PINB
        Click_Status status = Clicked(buttonTickResult_PB1);
        if(status == Click_Status_Pressed) canRelese = true;
        if(canRelese && status == Click_Status_Relesed){
            isOn = !isOn;
            changes++;
            canRelese = false;
        }
        if(!BIT_CHECK(PINB, BUTTON_PIN)){ // KLICKAD!
            isOn = !isOn;
        }
        if(old != changes){
            printf("%d\n", changes);
            old = changes;
        }
        if(isOn)
            BIT_SET(PORTB, LED_PIN); 
        else
            BIT_CLEAR(PORTB, LED_PIN); 
    }
    return 0;
}
