#include "backend.h"
#include "queue.h"
#include "lock.h"
#include "tcb.h"
#include "rcc.h"
#include "timx.h"
#include <stdint.h>

void systeminit(void){

    set_clock_128Mhz();

    // systic interrupt init
    STK->CTRL |= 0x6;
    STK->LOAD |= 0x7CFF;
    
    // priority set
    SCB->SHPR3 |= 0xE0F00000;
     
    RCC->APB1ENR1 |= (1 << 21);
    
    RCC->AHB2ENR |= 0x7;                   // enable GPIO ABC  clock 

    //i2c?
    // RCC->APB1ENR1 |= (1 << 21);
    // RCC->APB2ENR |= (1 << 11);              // Enable TIM1
    
    RCC->AHB2ENR |= GPIOA_EN|GPIOB_EN;      // enable GPIO AB 
    
    SetPinOutput(GPIOA, PA0);
    // enable i2c interrupt
    // NVIC->ISER0 |= 1<<31;
    
    ready_queue_init();
    priority_queue_init();
    transport_queue_init();

    // dispatcher_init();
    
    // usart_init(USART1, GPIOC,PC4|PC5,115200);
    InitBasicTIM();
    input_timer_init();
    output_timer_init();

    SetPinOutput(GPIOA,PA5);
    I2C_Init(I2C1, 0);
}
