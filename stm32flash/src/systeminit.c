#include "stm32f303.h"

//Initialize Timer, GPIO Circuitry.
//
// TIM2_CR2 Bit 7 has TIM1_CH selection

void SystemInit(void){

    //TIM2 ENCODer SEtup
    RCC->AHBENR |= 1<<17;                   // GPIOA clock enable
    SetPinAlternate(GPIOA,0x3);             // Set pins 0 & 1 to AF mode TIM2
    RCC->CR |= 1;                           // Enable HSI          
    AlternateFunctionSet(GPIOA,0x3,1);      // Set pins 0 & 1 to AF1 for TIM2
    TIM2->CR2 |= (1 << 7);                  // XOR 3 timer channels sent to TI1 input
    TIM2->SMCR |=0b0001;                    // Encoder mode 1 - Counter counts up/down on TI1FP1 edge depending on TI2FP2  level. Consider trigger selection to sync counter.
    TIM2->CCMR1 |= 0x101;                   // CC1 is input IC mapped on T1I, capture done every 2 events Note: CC1S bits are writable only when the channel is OFF (CC1E = 0 in TIMx_CCER). 
    TIM2->CCMR2 |= 0b101;                   // CC2 is input IC mapped on T1I, capture done every 2 events Note: CC1S bits are writable only when the channel is OFF (CC1E = 0 in TIMx_CCER). 
    TIM2->CCER |= 0b100010001;                  // Capture mode 1 & 2 enabled & sensitive to TIxFP1 rising edge(Encoder Mode) 
    
    //TIM1 PWM SEtup
    SetPinAlternate(GPIOA,0x100);           // set PA 8 to AF mode
    AlternateFunctionSet(GPIOA,1<<8,6);     // set PA 8 to AF6 Maps to TIM1_CH1 (check) 
    TIM1->PSC = 1;                         // Prescale TIM1 by 64
    TIM1->ARR = 2000;                      // Auto reload @ 10000
    TIM1->CCR1 = 100;                      // compare @ 5000
    TIM1->CCMR1 |= 0x60;                    // PWM Mode 1
    TIM1->CCER |= 1;                        // Refer to rm0316 Table 122 
    TIM1->BDTR |= 1<<15;                    // Main Output enable
    
    InitGenTIM(TIM2);                       // Enable TIM2 Counter
    InitAdvTIM(TIM1);                       // Enable TIM1 Counter
    RCC->APB1ENR |= (1 << 0);               // Enable TIM2
    RCC->APB2ENR |= (1 << 11);              // Enable TIM1
}
