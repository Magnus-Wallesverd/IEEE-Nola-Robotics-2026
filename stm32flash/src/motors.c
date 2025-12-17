#include "motors.h"
#include "timx.h"
#include "rcc.h"
#include <stdint.h>

Gen_TIM_TypeDef1* input_timers[] = {TIM2, TIM3, TIM4, TIM15};

void TIM1_UP_TIM16_IRQHandler(void){
    
    TIM16->SR = 0;      // clear flags
}

void input_timer_init(void){
    RCC->APB1ENR |= 0x7;        // Enable TIM 2, 3, 4
    RCC->APB2ENR |= (1 << 16);  // Enable TIM 15
    
    SetPinAlternate(GPIOA, 0x18CF);
    AlternateFunctionSet(GPIOA, 0x03, 1);     //TIM2
    AlternateFunctionSet(GPIOA, 0xC0, 2);     //TIM3
    AlternateFunctionSet(GPIOA, 0x1800, 10);  //TIM4
    AlternateFunctionSet(GPIOA, 0x0C, 9);     //TIM15

    // define number of input timers as 4
    for(int i = 0; i < 4; i++){
        input_timers[i]->SMCR |= 0x3;  // Encoder mode 3
        input_timers[i]->CCMR1 |= 0x101; // CC1 is input IC -> T1I
        input_timers[i]->CCER |= 0x11;   // Capture mode 1 & 2 enabled   
        /*input_timers[i]->DIER |= 0x6;*/
        input_timers[i]->CR1 |= 0b10000001; // enable timer
    }
}

void output_timer_init(void){

    RCC->APB2ENR |= (1 << 11)|(1 << 17);    // Enable TIM 1, 16

    //TIM1
    SetPinAlternate(GPIOC, 0xF);            
    AlternateFunctionSet(GPIOC, 0xF, 2);    // PC0-3 -> AF2   

    TIM1->PSC   |= 0;           //
    TIM1->ARR    = 7999;        // top
    TIM1->CCR1  |= 4000;        // compare ch1
    TIM1->CCMR1 |= 0x68;        // pwm 1 ch1
    TIM1->CCER  |= 1;           // enable CC
    TIM1->BDTR  |= 1<<15;       // Main Output enable
    TIM1->CR1 |= 0b10000001;    // Enable TIM1 counter
    
    //TIM16
    SetPinAlternate(GPIOB, 1<<8);
    AlternateFunctionSet(GPIOB, 1 << 8, 1);  // PB8 -> AF1
    
    TIM16->DIER  |= 1;
    TIM16->ARR    = 7999;
    TIM16->CCR1  |= 4000;
    TIM16->CCMR1 |= 0x68;
    TIM16->CCER  |= 1;

    TIM16->CR1 |= 0b10000001;
}

void update_speed(void){

}
