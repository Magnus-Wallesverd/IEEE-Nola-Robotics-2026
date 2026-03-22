#include "backend.h"
#include "queue.h"
#include "lock.h"
#include "tcb.h"
#include "rcc.h"
#include "timx.h"
#include <stdint.h>

void systeminit(void){

    // systic interrupt init
    STK->CTRL |= 0x6;
    STK->LOAD |= 0x1F3F;
    
    // priority set
    SCB->SHPR3 |= 0xE0F00000;
    
    RCC->CR |= HSION;
    while(!(RCC->CR & HSIRDY));

    RCC->CFGR |= HSI16_SEL;
    while(!(RCC->CFGR&HSI16_STATUS));


    //i2c?
    // RCC->APB1ENR1 |= (1 << 21);
    // RCC->APB2ENR |= (1 << 11);              // Enable TIM1
    
    RCC->AHB2ENR |= GPIOA_EN|GPIOB_EN;      // enable GPIO AB 
    
    SetPinOutput(GPIOA, PA0);
    // enable i2c interrupt
    // NVIC->ISER0 |= 1<<31;
    
    I2C_Init(I2C1, 0);
}
