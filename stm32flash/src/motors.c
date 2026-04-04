#include "motors.h"
#include "timx.h"
#include "rcc.h"
#include "lock.h"
#include "tcb.h"
#include "stmath.h"
#include "gpio.h"
#include "i2c.h"
#include "backend.h"
#include <stdint.h>
#include "task.h"

//works for now because motor task never quits
//still works because only 1 motor task gets called
TCB* motor_tcb;

uint32_t motor_timeout_counter = 0;
uint32_t MAXTIMEOUT = 40;

Gen_TIM_TypeDef1* input_timers[] = {TIM2, TIM3, TIM4};
int32_t ierr = 0;
int16_t prev2 = 0;
int16_t prev3 = 0;
int16_t prev4 = 0;
int16_t prev8 = 0;
int ovf = 0;
int16_t curr2 = 0;
int16_t curr3 = 0;
int16_t curr4 = 0;
int16_t curr8 = 0;
int16_t ref_h= 0;
int16_t curr_h = 0;
int16_t prev_h = 0;
int16_t error_h = 0;
int16_t target_h = 0;
uint8_t Kp_h = 2;
int16_t x =0;
int16_t y = 0;
int32_t pwm=0;
int16_t devi = 0;
int16_t measure2 = 0;
int16_t measure3 = 0;
int16_t measure4 = 0;
int16_t measure8 = 0;
int16_t error2 = 0;
int16_t error3 = 0;
int16_t error4 = 0;
int16_t error8 = 0;
int16_t target = 48 * 10*3;
uint8_t dir = 0;
uint32_t counter;
uint8_t Kp = 1;
int16_t measure_h = 0;
uint16_t A = 40;
uint16_t B = 3;
uint16_t C = 35;

uint8_t lateral_Kd = 60;
uint8_t lateral_Kp = 3;
uint8_t lateral_Ki = 4;

uint16_t hA = 120;
uint16_t hB = 100;
uint16_t hC =  30;

void TIM7_IRQHandler(void){
    TIM7->SR = 0;  // clear flags
    if(motor_tcb->state == BLOCKED){
        unblock(motor_tcb);
    }     
}


void input_timer_init(void){
    RCC->APB1ENR1 |= 0x7;        // Enable TIM 2, 3, 4
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
    
    // init_servo_TIM();

    RCC->APB2ENR |= (1 << 11)|(1 << 17)|(1 << 20);    // Enable TIM 1, 16, 20

    //TIM1 PWM
    SetPinAlternate(GPIOC, 0xF);            
    AlternateFunctionSet(GPIOC, 0xF, 2);    // PC0-3 -> AF2   
    
    SetPinAlternate(GPIOB, PB4);            
    AlternateFunctionSet(GPIOB, PB4, 1);    // PC0-3 -> AF2   

    //GPIO Control pins PC 8-12 PB 
    SetPinOutput(GPIOC, PC8|PC9|PC10|PC11|PC12);
    SetPinOutput(GPIOB, PB3|PB5);
    SetPinOutput(GPIOA, PA10);

    TIM1->CCMR1 |= 0x6868;      // pwm 1 CH 1,2
    TIM1->CCMR2 |= 0x6868;      // pwm 1 CH 3,4
    TIM1->PSC   |= 7;           //
    TIM1->ARR   = 0xFFFF;        // top
    TIM1->CCR1  = 0;        // compare ch1
    TIM1->CCR2  = 0;        // compare ch1
    TIM1->CCR3  = 0;        // compare ch1
    TIM1->CCR4  = 0;        // compare ch1
    TIM1->CCER  |= 0x1111;      // enable CC 1-4
    TIM1->BDTR  |= 1<<15;       // Main Output enable
    TIM1->CR1 |= 0b10000001;    // Enable TIM1 counter
    
    // TIM16->CCMR1 |= 0x68;
    // TIM16->PSC   |= 0;
    // TIM16->ARR    = 23999;
    // TIM16->CCR1  |= 0;
    // TIM16->CCER  |= 1;
    // TIM16->BDTR  |= 1<<15;       // Main Output enable
    // TIM16->CR1   |= 0b10000001;
    
}

void InitBasicTIM(void){
    RCC->APB1ENR1 |= 1 <<5;
    NVIC->ISER1 |= 1 <<23;
    // NVIC_IPR->IPR19 |= NVIC_IRQ_PRIORITY1 << 16;
    TIM7->DIER  |= 1;
    TIM7->PSC   |= 24*8;
    TIM7->ARR    = 7999;
    TIM7->CR1   |= 0b10000001;

}

void zero_timers(void){
    TIM2->CNT = 0;
    TIM3->CNT = 0;
    TIM4->CNT = 0;
    TIM8->CNT = 0;

    TIM1->CCR1 = 0;
    TIM1->CCR2 = 0;
    TIM1->CCR3 = 0;
    TIM1->CCR4 = 0;
}

void turn_off_motors(void){
    GPIOC->BSRR |= (INL1|INL2|INL3|INL4|INR3)<<16;
    GPIOB->BSRR |= (INR1|INR4) << 16;
    GPIOA->BSRR |= INR2 << 16;
}

void zerocounter(){
        
    TIM2->CNT = 0;
    TIM3->CNT = 0;
    TIM4->CNT = 0;
    TIM8->CNT = 0;
}

int step(void* args){

    motor_tcb = current_tcb;
    motor_timeout_counter = 0;

    int8_t data = *((uint8_t*) args);
    int16_t error = 0;

    zerocounter();

    const int16_t target2 = 48*(data); // convert cm to encoder counts
    int16_t curr_avg = 0;
    int16_t derr = 0;
    int16_t prev = 0;
    while(1){
        
        if(motor_timeout_counter > MAXTIMEOUT){
            zero_timers();
            turn_off_motors();
            return 0;
        }

        curr2 = TIM2->CNT;
        curr3 = TIM3->CNT;
        curr4 = TIM4->CNT;
        curr8 = TIM8->CNT;
        curr_avg = (curr3+curr4+curr8+curr2)/4;
        error = (target2 - curr_avg);
        derr = error - prev;
        ierr += error /C;
        pwm = error*A + B*derr + ierr;
        if(pwm<0){ 

            GPIOC->BSRR |= ((INL4|INL1|INR3)<<16)|((INL3|INL2)) ;
            GPIOB->BSRR |= INR3<<16|(INR1);
            GPIOA->BSRR |= INR2<<16;
            pwm *=-1;
        }
        else if(pwm>0){ //forward 
            GPIOC->BSRR |= (INL4|INL1|INR3)|((INL3|INL2) << 16) ;
            GPIOB->BSRR |= ((INR4|INR1) <<16);
            GPIOA->BSRR |= INR2;
        }
        if(pwm > 30000 || pwm < -30000){
            pwm = TIM1->ARR;
        }

        TIM1->CCR3 = pwm;
        TIM1->CCR2 = pwm;
        TIM1->CCR1 = pwm;
        TIM1->CCR4 = pwm;
        prev = error;
        if((error < 70 && error > -70) && derr ==0){
            zero_timers();
            turn_off_motors();
            return 1;
        }
        block();
        motor_timeout_counter++;
    }
}
void step2(int16_t args){
    motor_tcb = current_tcb;

    int16_t error = 0;
    motor_timeout_counter = 0;
    

    zerocounter();

    const int16_t target2 = 48*(args); // convert cm to encoder counts
    int16_t curr_avg = 0;
    int16_t derr = 0;
    int16_t prev = 0;
    while(1){
        curr_h = (i2c_rx_buffer[HEADING_MSB] << 8 | i2c_rx_buffer[HEADING_LSB]);
        if(motor_timeout_counter > MAXTIMEOUT){
            zero_timers();
            turn_off_motors();
            return ;
        }
        curr2 = TIM2->CNT;
        curr3 = TIM3->CNT;
        curr4 = TIM4->CNT;
        curr8 = TIM8->CNT;
        curr_avg = (curr3+curr4+curr8+curr2)/4;
        error = (target2 - curr_avg);
        derr = error - prev;
        ierr += error /C;
        pwm = error*A + B*derr + ierr;
        if(pwm<0){ 
            GPIOC->BSRR |= ((INL4|INL1|INR3)<<16)|((INL3|INL2)) ;
            GPIOB->BSRR |= INR3<<16|(INR1);
            GPIOA->BSRR |= INR2<<16;
            pwm *=-1;
        }
        else if(pwm>0){ //forward 
            GPIOC->BSRR |= (INL4|INL1|INR3)|((INL3|INL2) << 16) ;
            GPIOB->BSRR |= ((INR4|INR1) <<16);
            GPIOA->BSRR |= INR2;
        }
        if(pwm > 30000 || pwm < -30000){
            pwm = TIM1->ARR;
        }

        TIM1->CCR3 = pwm;
        TIM1->CCR2 = pwm;
        TIM1->CCR1 = pwm;
        TIM1->CCR4 = pwm;
        prev = error;
        if((error < 70 && error > -70) && derr ==0){
            zero_timers();
            turn_off_motors();
        }
        motor_timeout_counter++;
        devi += (target_h - curr_h)*derr; //endyne per second;
        block();
    }
}
void rotate2(int16_t args){
    motor_tcb = current_tcb;
    motor_timeout_counter = 0;

    curr_h = (i2c_rx_buffer[HEADING_MSB] << 8 | i2c_rx_buffer[HEADING_LSB]);
    measure_h = 0;
    while(1){
        if(motor_timeout_counter > MAXTIMEOUT){
            zero_timers();
            turn_off_motors();
            return ;
        }
        curr_h = (i2c_rx_buffer[HEADING_MSB] << 8 | i2c_rx_buffer[HEADING_LSB]);
        measure_h = args - curr_h;
        if(measure_h <= HEADING_MAX_VALUE/2) measure_h+=HEADING_MAX_VALUE;
        if(measure_h > HEADING_MAX_VALUE/2) measure_h-=HEADING_MAX_VALUE;   
        measure8 = (measure_h - prev_h);
        ierr += measure_h/(hC*dir);
        pwm = measure_h*hA + hB*measure8 + ierr;
        prev_h = measure_h;
        if(pwm <0){
            pwm*=-1;
            GPIOC->BSRR |= (INL4|INL1)<<16 |((INL3|INL2|INR3));
            GPIOB->BSRR |= (INR1|INR4)<<16;
            GPIOA->BSRR |= INR2;
        }
        else{
            GPIOC->BSRR |= (INL4|INL1)|((INL3|INL2|INR3)<<16);
            GPIOB->BSRR |= (INR1|INR4);
            GPIOA->BSRR |= INR2<<16;
        }
        if(pwm > 30000 || pwm < -30000){
            pwm = TIM1->ARR;
        }
        TIM1->CCR3 = pwm;
        TIM1->CCR2 = pwm;
        TIM1->CCR1 = pwm;
        TIM1->CCR4 = pwm;
        if((measure_h < 48 && measure_h > -48) && measure8 ==0){
            zero_timers();
            turn_off_motors();
            return;
        }
        motor_timeout_counter++;
        block();
    }
}

int rotate(void* args){
    motor_tcb = current_tcb;
    motor_timeout_counter = 0;

    int8_t data = *((uint8_t*) args);
    int16_t curr_h = (i2c_rx_buffer[HEADING_MSB] << 8 | i2c_rx_buffer[HEADING_LSB]);
    int16_t target = curr_h + data*16;
    int16_t measure_h = 0;
    while(1){

        if(motor_timeout_counter > MAXTIMEOUT){
            zero_timers();
            turn_off_motors();
            return 0;
        }

        curr_h = (i2c_rx_buffer[HEADING_MSB] << 8 | i2c_rx_buffer[HEADING_LSB]);
        measure_h = target - curr_h;
        if(measure_h <= HEADING_MAX_VALUE/2) measure_h+=HEADING_MAX_VALUE;
        if(measure_h > HEADING_MAX_VALUE/2) measure_h-=HEADING_MAX_VALUE;   
        measure8 = (measure_h - prev_h);
        ierr += measure_h/(hC*dir);
        pwm = measure_h*hA + hB*measure8 + ierr;
        prev_h = measure_h;
        if(pwm <0){
            pwm*=-1;
            GPIOC->BSRR |= (INL4|INL1)<<16 |((INL3|INL2|INR3));
            GPIOB->BSRR |= (INR1|INR4)<<16;
            GPIOA->BSRR |= INR2;
        }
        else{
            GPIOC->BSRR |= (INL4|INL1)|((INL3|INL2|INR3)<<16);
            GPIOB->BSRR |= (INR1|INR4);
            GPIOA->BSRR |= INR2<<16;
        }
        if(pwm > 30000 || pwm < -30000){
            pwm = TIM1->ARR;
        }
        TIM1->CCR3 = pwm;
        TIM1->CCR2 = pwm;
        TIM1->CCR1 = pwm;
        TIM1->CCR4 = pwm;
        if((measure_h < 48 && measure_h > -48) && measure8 ==0){
            zero_timers();
            turn_off_motors();
            return 1;
        }
        block();
        motor_timeout_counter++;
    }
}
int lateral_left(void* args){

    motor_tcb = current_tcb;
    uint8_t target = *((uint8_t*)args);
    motor_timeout_counter = 0;
    
    // turn_off_motors();
    // zero_counters();

    GPIOC->BSRR |= INL2|INL4;
    GPIOB->BSRR |= INR1|INR4;    
    while(1){

        if(motor_timeout_counter > MAXTIMEOUT){
            zero_timers();
            turn_off_motors();
            return 0;
        }

        curr2 = TIM2->CNT;
        curr3 = abs(TIM3->CNT);
        curr4 = TIM4->CNT;
        curr8 = abs(TIM8->CNT);
        
        error2+= (target-curr2)/lateral_Ki;
        error3+= (target-curr3)/lateral_Ki;
        error4+= (target-curr4)/lateral_Ki;
        error8+= (target-curr8)/lateral_Ki;

        int16_t output2 = lateral_Kd*(curr2-prev2) + lateral_Kp*(target - curr2) + (error2);
        int16_t output3 = lateral_Kd*(curr3-prev3) + lateral_Kp*(target - curr3) + (error3);
        int16_t output4 = lateral_Kd*(curr4-prev4) + lateral_Kp*(target - curr4) + (error4);
        int16_t output8 = lateral_Kd*(curr8-prev8) + lateral_Kp*(target - curr8) + (error8);

        if(target - curr2 < 0){
            zero_timers();
            turn_off_motors();
            return 1;
        } else {
            TIM1->CCR1 = output2;
            TIM1->CCR2 = output3;
            TIM1->CCR3 = output4;
            TIM1->CCR4 = output8;
        }
        
        prev2 = curr2;
        prev3 = curr3;
        prev4 = curr4;
        prev8 = curr8;
        block();
        motor_timeout_counter++;
    }   
}

int lateral_right(void* args){

    motor_tcb = current_tcb;
    uint8_t data = *((uint8_t*)args);
    
    int16_t target = data * 48;

    GPIOA->BSRR |= INR2;
    GPIOC->BSRR |= INL1|INL3|INR3;    
    
    while(1){

        if(motor_timeout_counter > MAXTIMEOUT){
            zero_timers();
            turn_off_motors();
            return 0;
        }
        curr2 = abs(TIM2->CNT);
        curr3 = TIM3->CNT;
        curr4 = abs(TIM4->CNT);
        curr8 = TIM8->CNT;
        
        error2+= (target-curr2)/lateral_Ki;
        error3+= (target-curr3)/lateral_Ki;
        error4+= (target-curr4)/lateral_Ki;
        error8+= (target-curr8)/lateral_Ki;

        int16_t output2 = lateral_Kd*(curr2-prev2) + lateral_Kp*(target - curr2) + (error2);
        int16_t output3 = lateral_Kd*(curr3-prev3) + lateral_Kp*(target - curr3) + (error3);
        int16_t output4 = lateral_Kd*(curr4-prev4) + lateral_Kp*(target - curr4) + (error4);
        int16_t output8 = lateral_Kd*(curr8-prev8) + lateral_Kp*(target - curr8) + (error8);

        if(target - curr2 < 0){
            TIM1->CCR1 = 0;
            TIM1->CCR2 = 0;
            TIM1->CCR3 = 0;
            TIM1->CCR4 = 0;
            zero_timers();
            turn_off_motors();
            return 1;
        }else {
            TIM1->CCR1 = output2;
            TIM1->CCR2 = output3;
            TIM1->CCR3 = output4;
            TIM1->CCR4 = output8;
        }
        
        prev2 = curr2;
        prev3 = curr3;
        prev4 = curr4;
        prev8 = curr8;
        block();
        motor_timeout_counter++;
    }

}

void global_pos(void* args){
    // one endyne is 1/48 cm
    // magneometer (0 to 5760) -> (0,2pi)
    // 1440   ->
    // 4320    -> -90 degree -> -x direction
    //  0  ->   0 degree -> +y direction
    //  5760 -.  180 degree   -y direction
    // int16_t x1 = 9999;
    // int16_t arg =0;
    // int16_t dispx = x1 -x;
    devi = 0;
    cpu_time(10);
    while(1);
    // target_h = 1440*(dispx > 0) + 4320*(dispx < 0 );
    target_h =0;
    // for(int i = 0; i < x1/11 + 1 ; i++){
    //     rotate2(target_h + (devi)/(40*9));
    //     step2(11);
    // }

}
