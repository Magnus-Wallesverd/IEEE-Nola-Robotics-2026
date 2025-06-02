#include "stm32f303.h"

//LED is  at PA5 

void main(void){
    
    RCC->AHBENR |= 0x00020000;

    // sets pin 5 to output mode
    //GPIOA->MODER |= 0x400;
    // Enable TIM2 (0x00000001) and GPIOA (0x00000004)
    RCC->APB1ENR |= 0x00000001;
    RCC->AHBENR |= 0x00000004;

    // Configure PA0 & PA1 as AF mode (0x00000002)
    GPIOA->MODER |= (0x00000002 << 0) | (0x00000002 << 2);
    GPIOA->AFRL |= (0x00000001 << 0) | (0x00000001 << 4);

    // Encoder mode 3 (0x0003), Capture TI1 & TI2 (0x0011)
    TIM2->SMCR |= 0x0003;
    TIM2->CCMR1 |= 0x0011;

    // Enable capture on CH1 & CH2 (0x0001)
    TIM2->CCER |= 0x0001;

    // Max counter value (0xFFFF)
    TIM2->ARR = 0xFFFF;

    // Start the timer (0x0001)
    TIM2->CR1 |= 0x0001;
    while(1){

        // output at pin 5 HIGH
        //GPIOA->ODR = TIM2->CNT<<3;



    }

}
