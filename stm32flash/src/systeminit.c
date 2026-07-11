
#include "stm32f303.h"
#include <stdint.h>

void systeminit(void){

    // systic interrupt init
    STK->CTRL |= 0x6;
    STK->LOAD |= 0x7CFF;
    
    // priority set
    SCB->SHPR3 |= 0xE0F00000;
     
    RCC->APB1ENR |= (1 << 21);
    // RCC->APB1ENR |= (1 << 2) | (1 << 21);               // Enable TIM4
    // RCC->APB2ENR |= (1 << 11);              // Enable TIM1
    
    RCC->AHBENR |= 0xE0000;                   // enable GPIO ABC  clock 

    ready_queue_init();
    priority_queue_init();
    
}
