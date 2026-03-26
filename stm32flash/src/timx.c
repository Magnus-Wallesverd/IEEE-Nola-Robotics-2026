#include <stdint.h>
#include "timx.h"
#include "backend.h"
#include "rcc.h"
#include "tcb.h"
#include "lock.h"
#include "motors.h"

uint32_t tim20_ovf;
uint32_t* tim20_ovf_p = &tim20_ovf;
Gen_TIM_TypeDef1* input_timers[] = {TIM2, TIM3, TIM4};

void TIM20_UP_IRQHandler(void){
    TIM20->SR = 0;  // clear flags
    tim20_ovf++;
    if(motor_tcb->state == BLOCKED){
        unblock(motor_tcb);
    }     
}

// void TIM7_IRQHandler(void){
//     TIM7->SR = 0;  // clear flags
//     tim7_ovf++;
//     if(motor_tcb->state == BLOCKED){
//         unblock(motor_tcb);
//     }     
// }

void input_timer_init(void){
    RCC->APB1ENR |= 0x7;        // Enable TIM 2, 3, 4
    RCC->APB2ENR |= (1 << 13);  // Enable TIM 8
    
    SetPinAlternate(GPIOA, 0x18C3);
    SetPinAlternate(GPIOC, 0xC0);
    AlternateFunctionSet(GPIOA, 0x03, 1);     //TIM2 PA 1&0
    AlternateFunctionSet(GPIOA, 0xC0, 2);     //TIM3 PA 6&7
    AlternateFunctionSet(GPIOA, 0x1800, 10);  //TIM4 PA 11&12
    AlternateFunctionSet(GPIOC, 0xC0, 4);     //TIM8 PC 6&7

    // define number of input timers as 3
    for(int i = 0; i < 3; i++){
        input_timers[i]->SMCR |= 0x3;  // Encoder mode 3
        input_timers[i]->CCMR1 |= 0x101; // CC1 is input IC -> T1I
        input_timers[i]->CCER |= 0x11;   // Capture mode 1 & 2 enabled   
        input_timers[i]->CR1 |= 0b10000001; // enable timer
    }

    TIM8->SMCR |= 0x3;
    TIM8->CCMR1 |= 0x101;
    TIM8->CCER |= 0x11;
    TIM8->BDTR  |= 1<<15;       // Main Output enable
    TIM8->CR1 |= 0b10000001;

}

void output_timer_init(void){
    
    // init_servo_TIM();

    RCC->APB2ENR |= (1 << 11)|(1 << 17)|(1 << 20);    // Enable TIM 1, 16, 20

    //TIM1 PWM
    SetPinAlternate(GPIOC, 0xF);            
    AlternateFunctionSet(GPIOC, 0xF, 2);    // PC0-3 -> AF2   
    

    TIM1->CCMR1 |= 0x6868;      // pwm 1 CH 1,2
    TIM1->CCMR2 |= 0x6868;      // pwm 1 CH 3,4
    TIM1->PSC   |= 31;           //
    TIM1->ARR   = 7999;        // top
    TIM1->CCR1  = 0;        // compare ch1         TIM3
    TIM1->CCR2  = 0;        // compare ch1         TIM8
    TIM1->CCR3  = 0;        // compare ch1         TIM4
    TIM1->CCR4  = 0;        // compare ch1         TIM2
    TIM1->CCER  |= 0x1111;      // enable CC 1-4
    TIM1->BDTR  |= 1<<15;       // Main Output enable
    TIM1->CR1 |= 0b10000001;    // Enable TIM1 counter
    
    TIM16->CCMR1 |= 0x68;
    TIM16->PSC   |= 19;
    TIM16->ARR    = 63999;
    TIM16->CCR1  |= 32000;
    TIM16->CCER  |= 1;
    TIM16->BDTR  |= 1<<15;       // Main Output enable
    TIM16->CR1   |= 0b10000001;

    // timer prio
    NVIC_IPR->IPR19 |= NVIC_IRQ_PRIORITY1 << 16;

    //TIM20 interrupt enable
    NVIC->ISER2 |= 1<<14;

    TIM20->DIER  |= 1;
    TIM20->CCMR1 |= 0x68;
    TIM20->PSC   |= 24*8;
    TIM20->ARR    = 7999;
    TIM20->CCR1  |= 4000;
    TIM20->CCER  |= 1;
    TIM20->BDTR  |= 1<<15;       // Main Output enable
    TIM20->CR1   |= 0b10000001;
}

// void InitBasicTIM(void){
//     RCC->APB1ENR1 |= 1 <<5;
//     NVIC->ISER1 |= 1 <<23;
//     // NVIC_IPR->IPR19 |= NVIC_IRQ_PRIORITY1 << 16;
//     TIM7->DIER  |= 1;
//     TIM7->PSC   |= 100;
//     TIM7->ARR    = 32000;
//     TIM7->CR1   |= 0b10000001;
//
// }
