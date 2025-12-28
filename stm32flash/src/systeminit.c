//Initialize Timer, GPIO Circuitry.

// TIM4_CR2 Bit 7 has TIM1_CH selection 
#include "stm32f303.h"

//#define DBGMCU_CR *((volatile uint32_t*) 0xE0042004)


void systeminit(void){
    uint8_t data = 0;

    // systic interrupt init
    STK->CTRL |= 0x6;
    STK->LOAD |= 0x1F3F;
    
    // priority set
    SCB->SHPR3 |= 0xE0F00000;
     
    RCC->APB1ENR |= (1 << 21);               // Enable TIM4
    // RCC->APB1ENR |= (1 << 2) | (1 << 21);               // Enable TIM4
    // RCC->APB2ENR |= (1 << 11);              // Enable TIM1
    
    RCC->AHBENR |= 0xE0000;                   // enable GPIO ABC  clock 

    SetPinAlternate(GPIOB,0xC0);            // Set pins 6 & 7 to AF mode I2c
    AlternateFunctionSet(GPIOB,0xC0,4);      // set pins PB 6&7 to AF4
    SetOutputType(GPIOB, 0xC0, 1);
    SetOutputSpeed(GPIOB,0xC0, 1);
    
    I2C_Init(I2C1, 0);
    I2C_Write_Read(I2C1, 0x28, 1, 0, data);
}
