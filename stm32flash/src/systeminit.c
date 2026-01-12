//Initialize Timer, GPIO Circuitry.

// TIM4_CR2 Bit 7 has TIM1_CH selection 
#include "stm32f303.h"

//#define DBGMCU_CR *((volatile uint32_t*) 0xE0042004)


void systeminit(void){
    uint8_t buf[I2C_BUFFER_SIZE];

    // systic interrupt init
    STK->CTRL |= 0x6;
    STK->LOAD |= 0x1F3F;
    
    // priority set
    SCB->SHPR3 |= 0xE0F00000;
     
    RCC->APB1ENR |= (1 << 21);
    // RCC->APB1ENR |= (1 << 2) | (1 << 21);               // Enable TIM4
    // RCC->APB2ENR |= (1 << 11);              // Enable TIM1
    
    RCC->AHBENR |= 0xE0000;                   // enable GPIO ABC  clock 

    NVIC->ISER0 |= 1<<31;

    SetPinAlternate(GPIOB,0xC0);            // Set pins 6 & 7 to AF mode I2c
    AlternateFunctionSet(GPIOB,0xC0,4);      // set pins PB 6&7 to AF4
    SetOutputType(GPIOB, 0xC0, 1);
    SetOutputSpeed(GPIOB,0xC0, 1);
    
    usart_init(USART1, GPIOC,PA4|PA5,9600);
    I2C_Init(I2C1, 0);
    Sensor_Read(I2C1);
    // I2C_Write_Read(I2C1, BNO055, 1, 0);
}
