// TODO hook motors and clamp the CCR integrator
// TODO implement 3 point backwards difference

#include "motors.h"
#include "timx.h"
#include "rcc.h"
#include "lock.h"
#include <stdint.h>

/* *
 * ENA - PC0 
 * ENB - PC1
 * IN1 - PC10
 * IN2 - PC11
 * IN3 - PC8
 * IN4 - PC9
 *
 * ENA - PC2
 * ENB - PC3
 * IN1 - PB1
 * IN2 - PB2
 * IN3 - PC4
 * IN4 - PC5
 *
 * ENCODERA - PA 6,7 
 * ENCODERB - PA 0,1
 *
 * ENCODERA - PA 11,12 
 * ENCODERB - PC 6,7
 *
 * */

//works for now because motor task never quits
TCB* motor_tcb;

Gen_TIM_TypeDef1* input_timers[] = {TIM2, TIM3, TIM4};

int16_t prev2 = 0;
int16_t prev3 = 0;
int16_t prev4 = 0;
int16_t prev8 = 0;

int16_t pwm=0;

int16_t measure2 = 0;
int16_t measure3 = 0;
int16_t measure4 = 0;
int16_t measure8 = 0;

int16_t error2 = 0;
int16_t error3 = 0;
int16_t error4 = 0;
int16_t error8= 0;

uint16_t target = 100;
uint8_t direction;

uint8_t Kp = 1;

void TIM1_UP_TIM16_IRQHandler(void){
    __asm volatile("BKPT #1"); // motor timer
    TIM16->SR = 0;  // clear flags
    if()
    unblock(motor_tcb);
    yield_isr();
}

void input_timer_init(void){
    RCC->APB1ENR |= 0x7;        // Enable TIM 2, 3, 4
    RCC->APB2ENR |= (1 << 13);  // Enable TIM 8
    
    SetPinAlternate(GPIOA, 0x18C3);
    SetPinAlternate(GPIOC, 0xC0);
    AlternateFunctionSet(GPIOA, 0x03, 1);     //TIM2 PA 1&0
    AlternateFunctionSet(GPIOA, 0xC0, 2);     //TIM3 PA 6&7
    AlternateFunctionSet(GPIOA, 0x1800, 10);  //TIM4 PA
    AlternateFunctionSet(GPIOC, 0xC0, 4);     //TIM8

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

    RCC->APB2ENR |= (1 << 11)|(1 << 17);    // Enable TIM 1, 16

    //TIM1 PWM
    SetPinAlternate(GPIOC, 0xF);            
    AlternateFunctionSet(GPIOC, 0xF, 2);    // PC0-3 -> AF2   

    //GPIO Control pins PC 8-12 PB 
    SetPinOutput(GPIOC, PC8|PC9|PC10|PC11|PC12);
    SetPinOutput(GPIOB, PB3|PB5);
    SetPinOutput(GPIOA, PA10);

    TIM1->CCMR1 |= 0x6868;      // pwm 1 CH 1,2
    TIM1->CCMR2 |= 0x6868;      // pwm 1 CH 3,4
    TIM1->PSC   |= 0;           //
    TIM1->ARR   = 7999;        // top
    TIM1->CCR1  = 0;        // compare ch1
    TIM1->CCR2  = 0;        // compare ch1
    TIM1->CCR3  = 0;        // compare ch1
    TIM1->CCR4  = 0;        // compare ch1
    TIM1->CCER  |= 0x1111;      // enable CC 1-4
    TIM1->BDTR  |= 1<<15;       // Main Output enable
    TIM1->CR1 |= 0b10000001;    // Enable TIM1 counter
    
    TIM16->DIER  |= 1;
    TIM16->CCMR1 |= 0x68;
    TIM16->PSC   |= 24;
    TIM16->ARR    = 7999;
    TIM16->CCR1  |= 4000;
    TIM16->CCER  |= 1;
    TIM16->BDTR  |= 1<<15;       // Main Output enable
    TIM16->CR1   |= 0b10000001;
}

void steps(int16_t target){
    int16_t curr4 = (TIM4->CNT);
    int16_t A = 1;
    int16_t B = 1;
    pwm = 0;
    measure4 = curr4 - prev4;
    prev4 = curr4;
    int16_t error = target - curr4;
    pwm = error*A + B*measure4;
    if(pwm<0){
        ResetPins(GPIOB, PB3|PB5);
        ResetPins(GPIOC, PC10|PC9);
    }
    else{
        PinWrite(GPIOC, PC11|PC8);
        PinWrite(GPIOB, PB3|PB5 );
    }
    if((pwm > TIM4->ARR) | (-1*pwm < TIM4->ARR)){
        pwm = TIM4->ARR;
    }
    TIM1->CCR1 = pwm;
    TIM1->CCR3 = pwm;
    TIM1->CCR2 = pwm;
    TIM1->CCR4 = pwm;

}

void set_speed(uint16_t target){
    
    int16_t curr2 = TIM2->CNT;
    int16_t curr3 = TIM3->CNT;
    int16_t curr4 = TIM4->CNT;
    int16_t curr8 = TIM8->CNT;

    measure2 = curr2 - prev2;
    measure3 = curr3 - prev3;
    measure4 = curr4 - prev4;
    measure8 = curr8 - prev8;

    prev2  = curr2; 
    prev3  = curr3;
    prev4  = curr4;
    prev8  = curr8;
    
    error2 = target - measure2;
    error3 = target - measure3;
    error4 = target - measure4;
    error8 = target - measure8;
    
    TIM1->CCR1 += (Kp*error2) * !(measure2 < -200 ||measure2 > 400);
    TIM1->CCR2 += (Kp*error8) * !(measure8 < -200 ||measure8 > 400);
    TIM1->CCR3 += Kp*error4   * !(measure4 < -200 ||measure4 > 200);
    TIM1->CCR4 += Kp*error3   * !(measure3 < -200 ||measure3 > 200);

}

void motor_wrapper(void* args){
    (void) args;
    motor_tcb = current_tcb;
    input_timer_init();
    output_timer_init();
    while(1){
        set_speed(target);
        __asm volatile("BKPT #2"); //block
        block();
    }
}
