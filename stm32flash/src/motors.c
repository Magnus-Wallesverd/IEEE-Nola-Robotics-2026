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
TCB* servo_tcb;

uint8_t geo_counter = 0;
uint8_t turn_counter = 0;

uint32_t motor_timeout_counter = 0;
uint32_t MAXTIMEOUT = 120;

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

void servo(void* args){
    
    uint32_t timer = ((servo_payload*)args)->timx;
    uint8_t angle = ((servo_payload*)args)->angle;

    // if(timx_1){
    //
    // }

    uint32_t min = (TIM16->ARR+1)/20;
    uint32_t max = (TIM16->ARR+1)/5;

    if(angle > 180){
        angle = 180;
    }
    TIM16->CCR1 =  min + ((max - min) * angle) / 180;
}

servo_payload default_pos = {.timx = (uint32_t)TIM16, .angle = 90};
servo_payload mag_pos = {.timx = (uint32_t)TIM16, .angle = 0};
servo_payload nonmag_pos = {.timx = (uint32_t)TIM16, .angle = 180};

servo_payload right_gate = {.timx = (uint32_t)TIM15, .angle = 0};
servo_payload left_gate = {.timx = (uint32_t)TIM15, .angle = 0};

void sorting_sm(){
   servo_tcb = current_tcb;
   while(1){
       servo((void*)&default_pos);
       for(int i = 0; i < 40; i++){ block(); }

       if(mag_data[0] > 200 || mag_data[0] < 200 
               || mag_data[1] > 200 || mag_data[1] < 200
               || mag_data[2] > 200 || mag_data[2] < 200) {
           servo((void*)&mag_pos);
           geo_counter++;
           turn_counter++;
       } else {
           servo((void*)&nonmag_pos);
           turn_counter++;
       }

       if(geo_counter > 5){ // open geo gate, need to reset
           servo((void*)&left_gate);
           geo_counter = 0;
       }

       if(turn_counter > 15){ // open neb gate, need to reset
           servo((void*)&right_gate);
           turn_counter = 0;
       }

       for(int i = 0; i < 40; i++){ block(); }
   }
}

void turn_on_motors(void){

    GPIOC->BSRR |= (INL4|INL1|INR3)|((INL3|INL2) << 16) ;
    GPIOB->BSRR |= ((INR4|INR1) <<16);
    GPIOA->BSRR |= INR2;
}

void turn_on_motors_b(void){
    GPIOC->BSRR |= ((INL4|INL1|INR3)<<16)|((INL3|INL2)) ;
    GPIOB->BSRR |= INR4|(INR1);
    GPIOA->BSRR |= INR2<<16;
}
void turn_off_motors(void){
    GPIOC->BSRR |= (INL1|INL2|INL3|INL4|INR3)<<16;
    GPIOB->BSRR |= (INR1|INR4) << 16;
    GPIOA->BSRR |= INR2 << 16;
}

void jump_start(uint8_t speed, int16_t dist){
    uint32_t tick = *tim20_ovf_p;
    if(dist < 0){
        turn_on_motors_b();
    }else{
        turn_on_motors();      
    }
    TIM1->CCR1 = 7999;
    TIM1->CCR2 = 7999;
    TIM1->CCR3 = 7999;
    TIM1->CCR4 = 7999;
    while(*tim20_ovf_p < tick + 3 - speed);

}

int32_t clamp_max_pwm(int32_t pwm, uint8_t speed){
    if(pwm < 0){
        pwm *= -1;
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

int step3(void* args){

    int16_t dist = ((motor_payload*)args)->args;
    uint8_t speed = ((motor_payload*)args)->speed;

    
    int16_t error = 0;
    int16_t tof_error = 0;
    int16_t total_error = 0;
    motor_timeout_counter = 0;
    
    zero_CNT();

    const int16_t target2 = (48*(dist)); // convert cm to encoder counts
    int16_t curr_avg = 0;
    int16_t derr = 0;
    int16_t prev = 0;
    
    int32_t pwm2 = 0;
    int32_t pwm3 = 0;
    int32_t pwm4 = 0;
    int32_t pwm8 = 0;
    
    uint16_t* tof_p = ToF_Distance_p;
    int16_t  tof_live;

    // while(*tof_p == 0){
    //     block();
    // }

    uint32_t tof_target = (*tof_p*480) - target2; 

    jump_start(speed, dist);

    while(1){
        if(*tof_p/10 < 30){
            // turn_off_motors();
            lock_motors();
            return 0 ;
        }
        if(motor_timeout_counter > MAXTIMEOUT){
            // zero_CCR();
            turn_off_motors();
            return 0 ;
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
            return 1;
        }
        motor_timeout_counter++;
        block();
    }
    while(1);
}

int rotate3(void* args){
    motor_tcb = current_tcb;

    int16_t angle = ((motor_payload*)args)->args;
    
    motor_timeout_counter = 0;
    zero_CNT();

    measure_h = 0;
    while(1){
        if(motor_timeout_counter > MAXTIMEOUT){
            zero_CCR();
            turn_off_motors();
            return 0;
        }
        measure_h = angle - *curr_h;
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
            return 1;
        }
        motor_timeout_counter++;
        block();
    }
    while(1);
}

int step2(int16_t args,uint8_t speed){
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

    while(1){
        tof_p2 = (int16_t) *tof_p;
        if(*tof_p/10 < 30){
            // turn_off_motors();
            lock_motors();
            return 0 ;
        }
        if(motor_timeout_counter > MAXTIMEOUT){
            // zero_CCR();
            turn_off_motors();
            return 0 ;
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
            return 1;
        }
        motor_timeout_counter++;
        block();
    }
}
int rotate2(int16_t args){
    motor_tcb = current_tcb;
    motor_timeout_counter = 0;
    zero_CNT();

    measure_h = 0;
    while(1){
        if(motor_timeout_counter > MAXTIMEOUT){
            zero_CCR();
            turn_off_motors();
            return 0;
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
            return 1;
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

void relative_pos2(void* args){

    int16_t x = ((cart_coords*)args)->x;
    int16_t y = ((cart_coords*)args)->y;
    target_h = 1440*(x>0) + 4320*(x <0);
    rotate2(target_h);
    step2(abs(x), 1);
    target_h = 0*(y>0) + 2880*(y <0);
    rotate2(target_h);
    step2(abs(y), 1);
}

int relative_pos(int16_t x1, int16_t y1){
    // one endyne is 1/48 cm
    // magneometer (0 to 5760) -> (0,2pi)
    // 1440   ->  90 degree +x direction
    // 4320    -> -90 degree -> -x direction
    //  0  ->   0 degree -> +y direction
    //  2880 -.  180 degree   -y direction
    //
    target_h = 1440*(x1>0) + 4320*(x1 <0);
    rotate2(target_h);
    step2(abs(x1), 2);
    target_h = 0*(y1>0) + 2880*(y1 <0);
    rotate2(target_h);
    step2(abs(y1), 2);

    return 1;

}

