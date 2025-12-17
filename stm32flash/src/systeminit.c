//Initialize Timer, GPIO Circuitry.

// TIM4_CR2 Bit 7 has TIM1_CH selection 
#include "stm32f303.h"

//#define DBGMCU_CR *((volatile uint32_t*) 0xE0042004)


void systeminit(void){
    
    // systic interrupt init
    STK->CTRL |= 0x6;
    STK->LOAD |= 0x1F3F;
    
    // priority set
    SCB->SHPR3 |= 0xE0F00000;
    
    // enable gpio clocks
    RCC->AHBENR |= (0xE << 16); // enable GPIO A,B,C clock 
    
    SetPinOutput(GPIOA, 0x20);                // OutputMode GPIOA
    
    intput_timer_init();
    output_timer_init();
}
