#include "stm32f303.h"

void systeminit(void){

    // systic interrupt init
    STK->CTRL |= 0x6;
    STK->LOAD |= 0x3D08FF;
    
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
    I2C_Init(I2C1, 0);

    // enable timer 16 interrupt
    // NVIC->ISER0 |= (1 << 25);
 
    // usart_init(USART1, GPIOC,PC4|PC5,115200);
}
