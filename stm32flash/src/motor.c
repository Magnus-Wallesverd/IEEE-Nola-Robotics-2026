#include "stm32f303.h"
#include <stdint.h>
short int speed =0;
short int d1=0;
int target = 0;
void update_speed(void* args){
    (void)args;
    uint32_t t1 = 0;
    t1 = get_global_tick();
    if(t1%8 == 1){
        speed = 10*(twos16Bit(TIM4->CNT) - d1);
        d1= twos16Bit(TIM4->CNT);
    }
    // speed = TIM4->CNT - d1;
    // d1 = TIM4->CNT;
}

void motorcontrol(void* args){
    (void)args;
    //int t1 =0;
    int pwm=0;
    int err=0;
    int ierr=0;
    int A=1;
    int B=-400;
    int C = 7000;
    int t1=0;
    while(1){  
        target = 10000;
        t1 += 1;
        err = target - twos16Bit(TIM4->CNT);
        ierr += err/20;
        pwm = (err)/(A)+B*(twos16Bit(speed))+ierr/C;
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

    } 
}
