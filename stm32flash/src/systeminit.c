//Initialize Timer, GPIO Circuitry.
//
// TIM4_CR2 Bit 7 has TIM1_CH selection
#include "stm32f303.h"

//#define DBGMCU_CR *((volatile uint32_t*) 0xE0042004)

void systeminit(void){
    
    // systic interrupt init
    STK->CTRL |= 0x6;

    // System clock time interval
    STK->LOAD |= 0x3E7F;
    
    // priority set
    SCB->SHPR3 |= 0xE0F00000;
    
}
