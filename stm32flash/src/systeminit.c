
#include "stm32f303.h"
#include "rcc.h"
#include <stdint.h>

void systeminit(void){

    set_clock_64Mhz();

    // systic interrupt init
    STK->CTRL |= 0x6;
    STK->LOAD |= 0xF9FF;
    
    // priority set
    SCB->SHPR3 |= 0xE0F00000;
     
    RCC->APB1ENR |= (1 << 21);
    // RCC->APB1ENR |= (1 << 2) | (1 << 21);               // Enable TIM4
    // RCC->APB2ENR |= (1 << 11);              // Enable TIM1
    
    RCC->AHBENR |= 0xE0000;                   // enable GPIO ABC  clock 


    // enable timer 16 and 17 interrupt
    NVIC->ISER0 |= (1 << 25) | 1 << 26;
 
    // enable i2c interrupt
    NVIC->ISER0 |= 1<<31;
    
    ready_queue_init();
    priority_queue_init();
    
    // enable timer 16 interrupt
    // NVIC->ISER0 |= (1 << 25);
 
    // usart_init(USART1, GPIOC,PC4|PC5,115200);
}
