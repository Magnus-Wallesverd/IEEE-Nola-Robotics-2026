//Initialize Timer, GPIO Circuitry.
//
// TIM3_CR2 Bit 7 has TIM1_CH selection 
#include "stm32f303.h"

#define DBGMCU_CR *((volatile uint32_t*) 0xE0042004)


void systeminit(void){
    DBGMCU_CR |= 3;
    RCC->APB1ENR |= (1 << 1);               // Enable TIM3
    RCC->APB2ENR |= (1 << 11);              // Enable TIM1
    
    //TIM3 ENCODer SEtup
    RCC->AHBENR |= 0xA0000;                   // GPIO A&C  clock enable
    SetPinAlternate(GPIOC,0xC0);// Set pins 0 & 1 to AF mode TIM3
    AlternateFunctionSet(GPIOC,0xC0,2);      // Set pins PC0 & PC1 to AF2 for TIM3
    TIM3->ARR = 0xffff;
    TIM3->SMCR |=0b0011;                    // Encoder mode 1 - Counter counts up/down on TI1FP1 edge depending on TI2FP2  level. Consider trigger selection to sync counter.
    TIM3->CCMR1 |= 0x101;                   // CC1 is input IC mapped on T1I, capture done every 2 events Note: CC1S bits are writable only when the channel is OFF (CC1E = 0 in TIMx_CCER). 
    //TIM3->CCMR2 |= 0b101;                   // CC2 is input IC mapped on T1I, capture done every 2 events Note: CC1S bits are writable only when the channel is OFF (CC1E = 0 in TIMx_CCER). 
    TIM3->CCER |= 0x11;                      // Capture mode 1 & 2 enabled & sensitive to TIxFP1 rising edge(Encoder Mode) 
    TIM3->DIER |= 0x6;
    
    //TIM1 PWM SEtup
    SetPinAlternate(GPIOA,0x100);           // set PA 8 to AF mode
    AlternateFunctionSet(GPIOA,1<<8,6);     // set PA 8 to AF6 Maps to TIM1_CH1 (check) 
    TIM1->PSC |= 0;                         // TIMx_CNT = f_ckpdc/(PSC+1)
    TIM1->ARR = 0xFFF;                      // Auto reload @ 10000
    TIM1->CCR1 |= 0x800;                    // compare @ 5000
    TIM1->CCMR1 |= 0x60;                    // PWM Mode 1
    TIM1->CCER |= 1;                        // Refer to rm0316 Table 122 
    TIM1->BDTR |= 1<<15;                    // Main Output enable
    
    TIM3->CR1 |= 0b10000001;                // Enable TIM3 Counter 
    TIM1->CR1 |= 0b10000001;                // Enable TIM1 Counter 
}
