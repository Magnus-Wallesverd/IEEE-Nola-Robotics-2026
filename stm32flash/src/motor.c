#include "stm32f303.h"
#include <stdint.h>
double speed =0;
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
    double err=0;
    double ierr=0;
    double A=10;
    double a1 = 0;
    double b1 = 0;
    double c1 =0;
    double B=10;
    double C = 1000;
    double errs=0;
    double temp=0;
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
	if(get_global_tick() > t2+1000){
	    temp = A;
	    A = abs( a1 + 0.5*(err+errs)/(A-a1));
	    a1 = temp;
	    temp = B;
	    B = abs(b1 - (err-errs)/(B-b1));
	    b1 = temp;
	    temp = C;
	    C = abs(c1 + 0.3*(err+errs)/(C-c1));
	    c1 = temp;

	    TIM1->CCR1 = 0; 
	    pwm = 0;
	    TIM4->CNT = 0;
	    for(int k=0; k< 1000; k++){

	    }
	    t2 = get_global_tick() + 1000;
	}
    } 
}
