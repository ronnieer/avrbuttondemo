#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <stdio.h>
#include <stdbool.h>
#include "uart.h"
#include "millis.h"

// https://wokwi.com/projects/374741122606566401

// B (digital pin 8 to 13)
// C (analog input pins)
// D (digital pins 0 to 7)
#define LED_PIN 0
#define MOTIONSENSOR_PIN 2

#define BIT_SET(a, b) ((a) |= (1ULL << (b)))
#define BIT_CLEAR(a,b) ((a) &= ~(1ULL<<(b)))
#define BIT_FLIP(a,b) ((a) ^= (1ULL<<(b)))
#define BIT_CHECK(a,b) (!!((a) & (1ULL<<(b)))) 




volatile bool isOn = false;


ISR(INT0_vect){
    isOn = !isOn;
}

int main(void){
    init_serial();

//    millis_init();

    EICRA |= (1 << ISC01) ||  (1 << ISC00);
    EIMSK |= (1 << INT0  );                                 /* enable INT0 */
    sei();

    BIT_SET(DDRB,LED_PIN); //OUTPUT MODE

    //Sätt till INPUT_PULLUP
    BIT_CLEAR(DDRD,MOTIONSENSOR_PIN); // INPUT MODE
    BIT_SET(PORTD,MOTIONSENSOR_PIN); 


    // DATA DIRECTION REGISTER = avgör mode
    // om output så skickar vi  1 eller 0 på motsvarande pinne på PORT
    // om input så läser vi  1 eller 0 på motsvarande pinne på PIN
    while(1){

        if(isOn)
            BIT_SET(PORTB, LED_PIN); 
        else
            BIT_CLEAR(PORTB, LED_PIN); 

    }
    return 0;
}
