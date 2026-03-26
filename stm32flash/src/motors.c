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

//works for now because motor task never quits
//still works because only 1 motor task gets called
TCB* motor_tcb;

uint32_t motor_timeout_counter = 0;
uint32_t MAXTIMEOUT = 320;

int16_t delta_tof;

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
int16_t* curr_h = &bno_heading;
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

uint16_t A = 5;
uint16_t B = 1;
uint16_t C = 14000;

uint8_t lateral_Kd = 60;
uint8_t lateral_Kp = 3;
uint8_t lateral_Ki = 4;

uint16_t hA = 15;
uint16_t hB = 10;
uint16_t hC =  300;

enum speed {
    SLOW = 1,
    MEDIUM,
    FAST
};

uint16_t speed_arr[] = {1499,3999,7999};
uint16_t kd_arr[] = {10,23,23};

void motor_init(void){
    //GPIO Control pins PC 8-12 PB 
    SetPinOutput(GPIOC, PC8|PC9|PC10|PC11|PC12);
    SetPinOutput(GPIOB, PB3|PB5);
    SetPinOutput(GPIOA, PA10);
}

void turn_on_motors(void){

    GPIOC->BSRR |= (INL4|INL1|INR3)|((INL3|INL2) << 16) ;
    GPIOB->BSRR |= ((INR4|INR1) <<16);
    GPIOA->BSRR |= INR2;
}

void turn_off_motors(void){
    GPIOC->BSRR |= (INL1|INL2|INL3|INL4|INR3)<<16;
    GPIOB->BSRR |= (INR1|INR4) << 16;
    GPIOA->BSRR |= INR2 << 16;
}

void jump_start(uint8_t speed){
    uint32_t tick = *tim20_ovf_p;
    turn_on_motors();      
    TIM1->CCR1 = 7999;
    TIM1->CCR2 = 7999;
    TIM1->CCR3 = 7999;
    TIM1->CCR4 = 7999;
    while(*tim20_ovf_p < tick + 3 - speed);

}

int32_t clamp_max_pwm(int32_t pwm, uint8_t speed){
    if(pwm < 0){
        pwm = 0;
    }
    if(abs_32(pwm) > speed_arr[speed-1]){
        pwm = speed_arr[speed-1];
    }
    return pwm;
}

void zero_CCR(void){

    TIM1->CCR1 = 0;
    TIM1->CCR2 = 0;
    TIM1->CCR3 = 0;
    TIM1->CCR4 = 0;
}

void zero_CNT(){
        
    TIM2->CNT = 0;
    TIM3->CNT = 0;
    TIM4->CNT = 0;
    TIM8->CNT = 0;
}

void lock_motors(void){
    GPIOC->BSRR |= (INL1|INL2|INL3|INL4|INR3);
    GPIOB->BSRR |= (INR1|INR4);
    GPIOA->BSRR |= INR2;

    TIM1->CCR1 = 7999;
    TIM1->CCR2 = 7999;
    TIM1->CCR3 = 7999;
    TIM1->CCR4 = 7999;
}

void step2(int16_t args,uint8_t speed){
    motor_tcb = current_tcb;
    
    int16_t error = 0;
    int16_t tof_error = 0;
    int16_t total_error = 0;
    motor_timeout_counter = 0;
    
    zero_CNT();

    const int16_t target2 = (48*(args)); // convert cm to encoder counts
    int16_t curr_avg = 0;
    int16_t derr = 0;
    int16_t prev = 0;
    
    int32_t pwm2 = 0;
    int32_t pwm3 = 0;
    int32_t pwm4 = 0;
    int32_t pwm8 = 0;
    
    uint16_t* tof_p = ToF_Distance_p;
    int16_t  tof_live;

    int16_t tof_p2 = (int16_t) *tof_p;
    while(*tof_p == 0){
        block();
    }

    uint32_t tof_target = (*tof_p*480) - target2; 

    jump_start(speed);

    while(1){
        tof_p2 = (int16_t) *tof_p;
        if(*tof_p/10 < 30){
            // turn_off_motors();
            lock_motors();
            return ;
        }
        if(motor_timeout_counter > MAXTIMEOUT){
            // zero_CCR();
            turn_off_motors();
            return ;
        }
        curr2 = TIM2->CNT + 180;
        curr3 = TIM3->CNT - 83;
        curr4 = TIM4->CNT + 35;
        curr8 = TIM8->CNT - 39;

        tof_error = -1*(tof_target - *tof_p*480);
        
        curr_avg = ((curr3)+curr4+(curr8)+curr2)/4;
        error = target2 - curr_avg;
        
        // error2 = target2 - curr2;
        // error3 = target2 - curr3;
        // error4 = target2 - curr4;
        // error8 = target2 - curr8;
        
        // total_error = (TOF_FILTER_WEIGHT*tof_error + ENCODER_FILTER_WEIGHT*error)/100;
        total_error = error;        
        derr = total_error - prev;
        ierr += total_error /C;
        pwm = total_error*A + kd_arr[speed-1]*derr + ierr;

        pwm2 = total_error*A + kd_arr[speed-1]*derr + ierr;
        pwm3 = total_error*A + kd_arr[speed-1]*derr + ierr;
        pwm4 = total_error*A + kd_arr[speed-1]*derr + ierr;
        pwm8 = total_error*A + kd_arr[speed-1]*derr + ierr;

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
        
        pwm2 = clamp_max_pwm(pwm2, speed);
        pwm3 = clamp_max_pwm(pwm3, speed);
        pwm4 = clamp_max_pwm(pwm4, speed);
        pwm8 = clamp_max_pwm(pwm8, speed);

        TIM1->CCR1 = pwm3;
        TIM1->CCR2 = pwm8;
        TIM1->CCR3 = pwm4;
        TIM1->CCR4 = pwm2;

        prev = total_error;

        if((total_error < 70 && total_error > -70) && derr ==0){
            zero_CCR();
            turn_off_motors();
            return;
        }
        motor_timeout_counter++;
        block();
    }
}
void rotate2(int16_t args){
    motor_tcb = current_tcb;
    motor_timeout_counter = 0;
    zero_CNT();

    measure_h = 0;
    while(1){
        if(motor_timeout_counter > MAXTIMEOUT){
            zero_CCR();
            turn_off_motors();
            return ;
        }
        measure_h = args - *curr_h;
        if(measure_h <= HEADING_MAX_VALUE/2) measure_h+=HEADING_MAX_VALUE;
        if(measure_h > HEADING_MAX_VALUE/2) measure_h-=HEADING_MAX_VALUE;   
        measure8 = (measure_h - prev_h);
        ierr += measure_h/hC;
        pwm = measure_h*hA + hB*measure8 + ierr ;
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
        if(pwm > 8000){
            pwm = TIM1->ARR;
        }
        TIM1->CCR3 = pwm+1000;
        TIM1->CCR2 = pwm+1000;
        TIM1->CCR1 = pwm+1000;
        TIM1->CCR4 = pwm+1000;
        if((measure_h < 48 && measure_h > -48) && measure8 ==0){
            zero_CCR();
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
    uint8_t data = *((uint8_t*)args);

    int16_t target = *curr_h + data*16;
    measure_h = 0;
    while(1){
        if(motor_timeout_counter > MAXTIMEOUT){
            zero_CCR();
            turn_off_motors();
            return 0;
        }
        measure_h = target - *curr_h;
        if(measure_h <= HEADING_MAX_VALUE/2) measure_h+=HEADING_MAX_VALUE;
        if(measure_h > HEADING_MAX_VALUE/2) measure_h-=HEADING_MAX_VALUE;   
        measure8 = (measure_h - prev_h);
        ierr += measure_h/hC;
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
        if(pwm > 8000){
            pwm = TIM1->ARR;
        }
        TIM1->CCR3 = pwm;
        TIM1->CCR2 = pwm;
        TIM1->CCR1 = pwm;
        TIM1->CCR4 = pwm;
        if((measure_h < 48 && measure_h > -48) && measure8 ==0){
            zero_CCR();
            turn_off_motors();
            return 0;
        }
        motor_timeout_counter++;
        block();
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
            zero_CCR();
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
            zero_CCR();
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
            zero_CCR();
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
            zero_CCR();
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

void relative_pos(void* args){
    // one endyne is 1/48 cm
    // magneometer (0 to 5760) -> (0,2pi)
    // 1440   ->  90 degree +x direction
    // 4320    -> -90 degree -> -x direction
    //  0  ->   0 degree -> +y direction
    //  2880 -.  180 degree   -y direction
    //
    int16_t x1 = 1440;
    int16_t y1 = 30;
    uint32_t deg = 90;
    void* deg_p = &deg;
    // target_h = 0*(y1>0) + 2880*(y1 <0);
    // target_h = 1440*(x1>0) + 4320*(x1 <0);
    
    while(1){
        rotate2(0);
        step2(180,MEDIUM);
        rotate2(1440);
        step2(180,MEDIUM);
        rotate2(2880);
        step2(180,MEDIUM);
        rotate2(4320);
        step2(180,MEDIUM);
    }
    // targe0t_h = 1440*(dispx > 0) + 4320*(dispx < 0 );
    // rotate2(0);
    // for(int i = 0; i < x1/11 + 1 ; i++ ){
    //     rotate2(target_h + (devi)/(40*10));
    //     step2(30);
    // }
    // // rotate2(1440);
    // for(int i = 0; i < y1/11 + 1 ; i++ ){
    //     rotate2(1440 + (devi)/(40*10));
    //     step2(11);
    // }

}

