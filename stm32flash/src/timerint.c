#include "stmf303.h"

/*
 * Use rm0316.pdf for stm32F303RE
 * TIMx_SR or (ISR) contains the update interrupt flag @ 0x01
 * TIMx_EGR uses software to generate an interrupt signal @ 0x01
 * TIMx_PSC 16 bits for clock division
 * TIMx_ARR 
 *
 * RCC_APB1ENR enables peripherals TIM6 & TIM7 included
 *
 * */

void setup(void){
    RCC->APB1ENR |= 0x10;       // enable TIM6
    TIM6->CR1 |= 0x85;          // enable (auto-preload, URS, Counter)
    TIM6->DIER |=0x01;          // UIE (update interrupt enable)
    GPIOA->MODER |= 0x400;      // output mode on PA5 (LED2)
    
}

void main(void){
    setup();

}
