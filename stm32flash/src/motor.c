#include "stm32f303.h"
#include <stdint.h>
int speed =0;
short int d1=0;
int target = 0;
int targetspeed=0;
void update_speed(void* args){
    (void)args;
    uint32_t t1 = 0;
    t1 = get_global_tick();
    if(t1%8 == 1){
        speed = 5*(twos16Bit(TIM4->CNT) - d1);
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
    int A=10;
    int B=10;
    int C = 1000;
    int errs=0;
    int t3=0;
    uint32_t t2 =10000;
    target = 2000;
    targetspeed = 10;
    while(1){  
        t3 += 1;
        err = target - twos16Bit(TIM4->CNT);
	errs = targetspeed - (speed);
        ierr += err/20;
        pwm = (err)*(A)+B*(errs)+ierr/C;
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
void motorgym(void* args){
    int dpos[3] = {1 , 5, target};
    int dvel[3] = {10, 20, 0};
    int error[3] ={0,0,0};
    int pars[3] = {0,0,0};
    int err=0;
    int errs =0;
    int pwm =0;
    while(1){
	err = target- twos16bit(TIM4->CNT);
	errs = target - speed ;
	pwm = err*pars[0] + errs*pars[1];
	if(pwm <0){
	    PinWrite(GPIOA, 2);
	    ResetPins(GPIOA, 1);
	}
	else{
	    PinWrite(GPIOA, 1);
	    ResetPins(GPIOA, 2);
	}
	
        if(abs(pwm)> TIM1->ARR){
            pwm = TIM1->ARR; 
         }
	TIM1->CCR1 = abs(pwm);


    }



}
