//Initialize Timer, GPIO Circuitry.

// TIM4_CR2 Bit 7 has TIM1_CH selection 
#include "stm32f303.h"

//#define DBGMCU_CR *((volatile uint32_t*) 0xE0042004)


void systeminit(void){
    
    // systic interrupt init
    STK->CTRL |= 0x6;
    STK->LOAD |= 0x1F3F;    // default 1ms refresh 0x1F3F
    
    // priority set
    SCB->SHPR3 |= 0xE0F00000;
    
    
    RCC->APB1ENR |= (1 << 2);               // Enable TIM4
    RCC->APB2ENR |= (1 << 11);              // Enable TIM1
    
    //TIM4 ENCODer SEtup
    RCC->AHBENR |= 0xA0000;                   // enable GPIO A&C  clock 
    SetPinOutput(GPIOA, 0x23);                // OutputMode GPIOA
    SetPinAlternate(GPIOA,0x1800);            // Set pins 11 & 12 to AF mode TIM4
    AlternateFunctionSet(GPIOA,0x1800,10);      // Set pins PA11 & PA12 to AF10 for TIM4
    TIM4->SMCR |=0b0011;                    // Encoder mode 1 - Counter counts up/down on TI1FP1 edge depending on TI2FP2  level. Consider trigger selection to sync counter.
    TIM4->CCMR1 |= 0x101;                   // CC1 is input IC mapped on T1I, capture done every 2 events Note: CC1S bits are writable only when the channel is OFF (CC1E = 0 in TIMx_CCER). 
    TIM4->CCER |= 0x11;                      // Capture mode 1 & 2 enabled & sensitive to TIxFP1 rising edge(Encoder Mode) 
    TIM4->DIER |= 0x6;
    
    //TIM1 PWM SEtup
    SetPinAlternate(GPIOA,0x100);           // set PA 8 to AF mode
    AlternateFunctionSet(GPIOA,1<<8,6);     // set PA 8 to AF6 Maps to TIM1_CH1 (check) 
    TIM1->PSC   |= 1;                   // TIMx_CNT = f_ckpdc/(PSC+1)
    TIM1->ARR    = 300;                  // Auto reload @ 10000
    TIM1->CCR1  |= 50;                  // compare @ 5000
    TIM1->CCMR1 |= 0x68;                    // PWM Mode 1
    TIM1->CCER  |= 1;                       // Refer to rm0316 Table 122 
    //TIM1->DIER  |= 1<<0; // UIE
    TIM1->BDTR  |= 1<<15;                   // Main Output enable
    
    TIM4->CR1 |= 0b10000001;                // Enable TIM4 Counter 
    TIM1->CR1 |= 0b10000001;                // Enable TIM1 counter
    //TIM1->SR  &= 0;     // clear UIF
    
    //Motor Directional  




}
