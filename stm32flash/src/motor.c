#include "stm32f303.h"
#include <stdint.h>
short int speed =0;
short int d1=0;
void update_speed(void* args){
    (void)args;
    uint32_t t0 = 0;
    uint32_t t1 = get_global_tick();
    while(1){
        if((t1-t0) > MOTOR_DT){
            speed = (twos16Bit(TIM4->CNT) - d1)*100;
            d1= twos16Bit(TIM4->CNT);
            t0=t1;
        } else { yield(); }
    }
}

void motorcontrol(void* args){
    (void)args;
    int target = 2000;
    int t1 =0;
    int pwm=0;
    int A=3;
    int B=4;
    while(1){
	    pwm = A*(target-TIM4->CNT)+B*(speed);
        if(pwm <0){
            //toggle bit the rotate backward
        }
        else{
            //rotate forward.
        }
        if(pwm> TIM1->ARR){
            pwm = 0xFFFF;
         }
        else if(pwm < 0){
            pwm = 0;
        }
        TIM1->CCR1 = pwm;
    } 
}
