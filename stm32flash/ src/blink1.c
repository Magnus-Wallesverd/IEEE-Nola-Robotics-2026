#include "stm32f303.h"

//LED is  at PA5 

void main(void){
    
    RCC->AHBENR |= 0x00020000;

    // sets pin 5 to output mode
    GPIOA->MODER |= 0x400;

    while(1){

        // output at pin 5 HIGH
        GPIOA->BSRR = 0x20;

        for(volatile int i = 0; i <1000000; i++);

        GPIOA->BRR = 0x20;

        for(volatile int i = 0; i <1000000; i++);
    }

}
