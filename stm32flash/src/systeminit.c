#include "stm32f303.h"
#include <stdint.h>

void systeminit(void){

    set_clock_32Mhz();

    // systic interrupt init
    STK->CTRL |= 0x6;
    STK->LOAD |= 0x7CFF;
    // STK->LOAD |= 0x9C3FF;
    
    // priority set
    SCB->SHPR3 |= 0xE0F00000;
    
    RCC->AHBENR |= 0xE0000;                   // enable GPIO ABC  clock 
                                              
    // input_timer_init();
    // output_timer_init();

    ready_queue_init();
    // motor_queue_init();
    // motor_init();
    // priority_queue_init();
    // transport_queue_init();
    // dispatcher_init();
 

    // usart_init(USART1, GPIOC,PC4|PC5,115200);
    // I2C_Init(I2C1, 0);
    spi_init(SPI1, 1, DIV4, 1, 0, 0);
    ST7796S_setup(SPI1);
       
}
