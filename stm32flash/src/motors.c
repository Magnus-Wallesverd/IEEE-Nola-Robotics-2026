#include "motors.h"
#include "timx.h"
#include "rcc.h"
#include "lock.h"
#include "tcb.h"
#include "stmath.h"
#include <stdint.h>


//works for now because motor task never quits
TCB* motor_tcb;

Gen_TIM_TypeDef1* input_timers[] = {TIM2, TIM3, TIM4};
int16_t ierr = 0;
int16_t prev2 = 0;
int16_t prev3 = 0;
int16_t prev4 = 0;
int16_t prev8 = 0;

int16_t ref_h= 0;
int16_t curr_h = 0;
int16_t measure_h;
int16_t prev_h = 0;
int16_t error_h = 0;
// uint16_t target_h = 0;
uint8_t Kp_h = 2;

int16_t pwm=0;

int16_t measure2 = 0;
int16_t measure3 = 0;
int16_t measure4 = 0;
int16_t measure8 = 0;

int16_t error2 = 0;
int16_t error3 = 0;
int16_t error4 = 0;
int16_t error8 = 0;
// uint16_t target = 100;
uint8_t direction;

uint8_t Kp = 1;

void TIM1_UP_TIM16_IRQHandler(void){
    TIM16->SR = 0;  // clear flags
}

void TIM20_UP_IRQHandler(void){
    TIM20->SR = 0;  // clear flags
    if(motor_tcb->state == BLOCKED){
        unblock(motor_tcb);
    }

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
    TIM8->CR1 |= 0b10000;

}

void output_timer_init(void){

    RCC->APB2ENR |= (1 << 11)|(1 << 17)|(1 << 20);    // Enable TIM 1, 16, 20

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
    
    TIM20->DIER  |= 1;
    TIM20->CCMR1 |= 0x68;
    TIM20->PSC   |= 24;
    TIM20->ARR    = 7999;
    TIM20->CCR1  |= 4000;
    TIM20->CCER  |= 1;
    TIM20->BDTR  |= 1<<15;       // Main Output enable
    TIM20->CR1   |= 0b10000001;
}

void steps(int16_t target, int16_t dir){

    curr_h = (i2c_rx_buffer[HEADING_MSB] << 8 | i2c_rx_buffer[HEADING_LSB]);
    measure_h = target - curr_h;
    if(measure_h <= HEADING_MAX_VALUE/2) measure_h+=HEADING_MAX_VALUE;
    if(measure_h > HEADING_MAX_VALUE/2) measure_h-=HEADING_MAX_VALUE;

    int16_t curr2 = TIM2->CNT;
    int16_t curr3 = TIM3->CNT;
    int16_t curr4 = TIM4->CNT;
    int16_t curr8 = TIM8->CNT;

    int16_t curr_avg = 0;
    curr_avg = (curr4+curr8+curr2+curr3)/4;

    
    int16_t A = 2;
    int16_t B = 7;
    int16_t C = 30;
    pwm = 0;
    measure8 = curr_avg - prev4;
    prev4 = curr_avg;
    int16_t error = (target - curr_avg)*(!dir) + measure_h*(dir);
    ierr += error;
    pwm = error*(A*(!dir)+ 10*dir) - B*measure4 + ierr/C;




    if((pwm<0) & (!dir) ){ //reverse rotation
        //GPIOC->BSRR |= INL4;
        //GPIOB->BSRR |= INR4;
        //GPIOA->BSRR |= INR3;
        GPIOC->BSRR |= INL3|INL2|INR3;
        //GPIOB->BSRR |= INR2;


        GPIOC->BSRR |= (INL4|INL1) <<16;
        GPIOB->BSRR |= (INR4)<<16;
        GPIOB->BSRR |= INR1;
        GPIOA->BSRR |= INR2 << 16;
        //GPIOB->BSRR |= INR4 << 16;
        //GPIOA->BSRR |= INR3 << 16;
        pwm *=-1;
    }
    else if((pwm>0 )& (!dir)){ //forward rotation

        
        //GPIOC->BSRR |= INL3|INL2|INR1 << 16;
        //GPIOB->BSRR |= INR2 << 16;


        GPIOC->BSRR |= INL4|INL1 ;
        GPIOB->BSRR |= INR4|(INR1 <<16);
        GPIOA->BSRR |= INR2;
        GPIOC->BSRR |= (INL3|INL2|INR3 )<< 16;
        //GPIOB->BSRR |= INR4;
        //GPIOA->BSRR |= INR2;
    }

    if((pwm <0) & (dir)){
        pwm*=-1;
        GPIOC->BSRR |= (INL4|INL1|INR3) << 16;
        GPIOB->BSRR |= (INR1 <<16) | INR4;
        GPIOA->BSRR |= INR2;
        GPIOC->BSRR |= (INL3|INL2);


    }
    else if((pwm >0) & (dir)){

        GPIOC->BSRR |= INL4|INL1|INR3 ;
        GPIOB->BSRR |= (INR1) | (INR4 << 16);
        GPIOA->BSRR |= INR2 << 16;
        GPIOC->BSRR |= (INL3|INL2 )<< 16;


    }
    if((pwm > TIM4->ARR) | (-1*pwm < TIM4->ARR)){
        pwm = TIM4->ARR;
    }
    TIM1->CCR3 = pwm;
    TIM1->CCR2 = pwm;
    TIM1->CCR1 = pwm;
    TIM1->CCR4 = pwm;
    //set_speed(measure8);

}

// R = ch1,4 & L = ch 2,3 
void heading_correction(int16_t target_h){
    curr_h = (i2c_rx_buffer[HEADING_MSB] << 8 | i2c_rx_buffer[HEADING_LSB]);
    measure_h = curr_h - prev_h;
    if((!measure_h) & bno_flag){
        return;
    }
    if(curr_h > HEADING_MAX_VALUE - 160 && prev_h < 160 ){
        //undf
        // decrement right side wheels
        error_h = target_h - (HEADING_MAX_VALUE - measure_h);
        TIM1->CCR1 += Kp_h*error_h;
        TIM1->CCR4 += Kp_h*error_h;
        
    } else if(curr_h < 160 && prev_h > HEADING_MAX_VALUE - 160){
        //ovf
        // decrement left side wheels
        error_h = target_h - twos_compl16(HEADING_MAX_VALUE + measure_h);
        TIM1->CCR2 += Kp_h*error_h;
        TIM1->CCR3 += Kp_h*error_h;
    } else {
        error_h = target_h - measure_h;
        if(measure_h < 0){
            // error_h is positive
            // decrement right side wheels
            error_h*=-1;
            TIM1->CCR1 += Kp_h*error_h;
            TIM1->CCR4 += Kp_h*error_h;
        } else {
            // decrement left side wheels
            // error_h is negative
            Kp_h*error_h;
            TIM1->CCR2 += Kp_h*error_h;
            TIM1->CCR3 += Kp_h*error_h;
        }
    }
    prev_h = curr_h;
}

void correct_heading(int16_t target_h){
    curr_h = (i2c_rx_buffer[HEADING_MSB] << 8 | i2c_rx_buffer[HEADING_LSB]);
    measure_h = curr_h - prev_h;
    if(measure_h <= HEADING_MAX_VALUE/2) measure_h+=HEADING_MAX_VALUE;
    if(measure_h > HEADING_MAX_VALUE/2) measure_h-=HEADING_MAX_VALUE;
    
    error_h = measure_h;

    if(abs(error_h) < 16){
        return;
    } else if(error_h < 0){
        TIM1->CCR2 += Kp_h*error_h;
        TIM1->CCR3 += Kp_h*error_h;
    } else {
        TIM1->CCR1 += Kp_h*error_h;
        TIM1->CCR4 += Kp_h*error_h;
    }
    prev_h = curr_h;
}

void set_speed(uint16_t target){
    
    int16_t curr2 = TIM2->CNT;
    int16_t curr3 = TIM3->CNT;
    int16_t curr4 = TIM4->CNT;
    int16_t curr8 = TIM8->CNT;

    measure2 = curr2 - prev2;
    measure3 = curr3 - prev3;
    //measure4 = curr4 - prev4;
    //measure8 = curr8 - prev8;

    prev2  = curr2; 
    prev3  = curr3;
    //prev4  = curr4;
    //prev8  = curr8;
    
    error2 = target - measure2;
    error3 = target - measure3;
    //error4 = target - measure4;
    //error8 = target - measure8;
    
    TIM1->CCR1 += (Kp*error2) * !(measure2 < -200 ||measure2 > 400);
    //TIM1->CCR2 += (Kp*error8) * !(measure8 < -200 ||measure8 > 400);
    //TIM1->CCR3 += Kp*error4   * !(measure4 < -200 ||measure4 > 200);
    //TIM1->CCR4 += Kp*error3   * !(measure3 < -200 ||measure3 > 200);
}


void motor_wrapper(void* args){
    (void) args;
    motor_tcb = current_tcb;
    input_timer_init();
    output_timer_init();
    // while(i2c_rx_buffer[1]== 0){
    //     yield();
    // }
    ref_h = (i2c_rx_buffer[HEADING_MSB] << 8 | i2c_rx_buffer[HEADING_LSB]);
    prev_h = ref_h;
    // GPIOC->BSRR |= INL1|INL4;
    // GPIOB->BSRR |= INR4;
    // GPIOA->BSRR |= INR3;
    for(int i = 0; i <0x50000;i++);
    while(1){
        steps(45*16, 1);
        block();
    // GPIOC->BSRR |= (INL1|INL4)<<16;
    // GPIOB->BSRR |= (INR4)<<16;
    // GPIOA->BSRR |= (INR3)<<16;
    }
}
