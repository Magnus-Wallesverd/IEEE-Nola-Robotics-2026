#include "stm32f303.h"
#include <stdint.h>

void update_speed(void){

    speed = (TIM4->CNT - d1)*100;
    d1=TIM4->CNT;
}

void motorcontrol(void* args){
    (void)args;
    int target = 1000;
    int t1 =0;
    int pwm=0;
    int A=3;
    int B=4;
    while(1){
	    pwm = A*(target-TIM4->CNT)+B*(target-TIM4->CNT)*(speed);
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
