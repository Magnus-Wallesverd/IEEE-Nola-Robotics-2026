#include "stm32f303.h"

//Initialize Timer, GPIO Circuitry.

void systemInit(void){

    //TIM2 ENCODer SEtup
    RCC->AHBENR |= 1<<17;
    SetPinAlternate(GPIOA,0x3);
    RCC->CR |= 1; //enable HSI
    InitGenTIM(TIM2); //Turn on CLock
    AlternateFunctionSet(GPIOA,0x3,1);
    TIM2->SMCR |=0b0001;
    TIM2->CCER |= 0b10001;
    TIM2->CCMR1 |= 0x101;
    RCC->APB2ENR |= 1<<11;
    //
    //TIM1 PWM SEtup
    SetPinAlternate(GPIOA,0x100);
    AlternateFunctionSet(GPIOA,1<<8,1);
    InitAdvTIM(TIM1);
    TIM1->PSC = 64;
    TIM1->ARR = 10000;
    TIM1->CCR1 = 5000;
    TIM1->CCMR1 |= 0x60;
    TIM1->CCER |= 1;
    TIM1->BDTR |= 1<<15;


}
