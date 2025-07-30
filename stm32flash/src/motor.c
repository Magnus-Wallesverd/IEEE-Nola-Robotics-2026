#include "stm32f303.h"
#include <stdint.h>
short int speed =0;
short int d1=0;
void update_speed(void* args){
    (void)args;
    uint32_t t1 = 0;
    t1 = get_global_tick();
    if(t1%8 == 1){
        speed = (twos16Bit(TIM4->CNT) - d1);
        d1= twos16Bit(TIM4->CNT);
    }
    // speed = TIM4->CNT - d1;
    // d1 = TIM4->CNT;
}

void motorcontrol(void* args){
    (void)args;
    int target = -10000;
    //int t1 =0;
    int pwm=0;
    int A=5;
    int B=-2;
    int t1=0;
    while(1){
	    pwm = (target-twos16Bit(TIM4->CNT))/A+B*(speed);
        if(pwm <0){
            //toggle bit the rotate backward
            PinWrite(GPIOA, 2);
            ResetPins(GPIOA, 1);        
        }
        else{
            //rotate forward.
            PinWrite(GPIOA,1);
            ResetPins(GPIOA,2);
        }
        if(abs(pwm)> TIM1->ARR){
            pwm = TIM1->ARR;
         }
        TIM1->CCR1 = abs(pwm);

        if(get_global_tick() > t1){
            t1 += 5000;
            target += 5000;
        }
    } 
}
