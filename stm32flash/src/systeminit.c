#include "stm32f303.h"
#include <stdint.h>

void systeminit(void){

    set_clock_64Mhz();

    // systic interrupt init
    STK->CTRL |= 0x6;
    STK->LOAD |= 0xF9FF;
    
    // priority set
    SCB->SHPR3 |= 0xE0F00000;
    
    RCC->AHBENR |= 0xE0000;                   // enable GPIO ABC  clock 
                                              
    ready_queue_init();
    // priority_queue_init();
    // transport_queue_init();
    // dispatcher_init();
    
    usart_init(USART1, GPIOC,PC4|PC5,115200);

    input_timer_init();
    output_timer_init();

    SetPinOutput(GPIOA,PA5);
    I2C_Init(I2C1, STANDARD_MODE);
}
