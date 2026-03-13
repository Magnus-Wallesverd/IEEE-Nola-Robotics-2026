
#include "stm32f303.h"
#include <stdint.h>

void systeminit(void){

    set_clock_64Mhz();

    // systic interrupt init
    STK->CTRL |= 0x6;
    STK->LOAD |= 0xF9FF;
    
    // priority set
    SCB->SHPR3 |= 0xE0F00000;
     
    RCC->APB1ENR |= (1 << 21);
    
    RCC->AHBENR |= 0xE0000;                   // enable GPIO ABC  clock 

    // enable i2c interrupt
    NVIC->ISER0 |= 1<<31;

    //TIM20 interrupt enable
    NVIC->ISER2 |= 1<<14;
    
    ready_queue_init();
    priority_queue_init();
    transport_queue_init();

    dispatcher_init();
    
    usart_init(USART1, GPIOC,PC4|PC5,115200);

    input_timer_init();
    output_timer_init();

    SetPinOutput(GPIOA,PA5);
}
