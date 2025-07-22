<<<<<<< Updated upstream
#include "stm32f303.h"
#include <stdint.h>
#include "math.h"


extern uint32_t _task1_end;
uint32_t speed=0;
uint32_t d1=0;
uint32_t global_tick;
TCB _stcb[SIZE];
TCB *current_tcb;
TCB *next_tcb;

void tcbinit(void){
    
    // list of functions 
    void (*function_list[SIZE])(void *) = {
        my_task1,
        my_task2,
        my_task3,
        my_task4,
        my_task5,
        my_task6,
        my_task7,
        my_task8,
        my_task9,
        my_task10
    };

    for(int i = 0; i < SIZE; i++){
        
        // _stcb[i].sp = &(_task1_end)+ i*0x100 ;
        _stcb[i].context = 0;
        _stcb[i].function = function_list[i];
        _stcb[i].pid = i;
        _stcb[i].state = 0;
        _stcb[i].prio = 0xB;
        _stcb[i].flags = 0xC;
    }

}

void my_task1(void *ctx){
    (void)ctx;
    int target = 5000;
    int pwm=0;
    int A=2;
    int B=30;
    while(1){
	    pwm = A*(target-TIM4->CNT)+speed/B;
        if(pwm <0){
            //toggle bit the rotate backward
        }
        else{
            //rotate forward.
        }
        pwm = abs(pwm);
        if(abs(pwm) > TIM1->ARR){
            pwm = TIM1->ARR;
        }
        if(pwm < 300){
            pwm =0;
        }
        TIM1->CCR1 = pwm;
    } 
}

void update_speed(void){
    speed = (TIM4->CNT-d1);
    d1=TIM4->CNT;
}

void my_task2(void *ctx){
    (void)ctx;
    while (1){
        ResetPins(GPIOA,0x20);
    }
}

void my_task3(void *ctx){
    (void)ctx;
    while(1){
        PinWrite(GPIOA, 0x20);
    } 
}

void my_task4(void *ctx){
    (void)ctx;
    while (1){
        ResetPins(GPIOA,0x20);
    }
}

void my_task5(void *ctx){
    (void)ctx;
    while(1){
        PinWrite(GPIOA, 0x20);
    } 
}

void my_task6(void *ctx){
    (void)ctx;
    while (1){
        ResetPins(GPIOA,0x20);
    }
}

void my_task7(void *ctx){
    (void)ctx;
    while(1){
        PinWrite(GPIOA, 0x20);
    } 
}

void my_task8(void *ctx){
    (void)ctx;
    while (1){
        ResetPins(GPIOA,0x20);
    }
}

void my_task9(void *ctx){
    (void)ctx;
    while(1){
        PinWrite(GPIOA, 0x20);
    } 
}

void my_task10(void *ctx){
    (void)ctx;
    while (1){
        ResetPins(GPIOA,0x20);
    }
}

TCB* taskscheduler(void){
    next_tcb++;
    if(next_tcb >= &_stcb[SIZE]){
        next_tcb = _stcb;
    }
    return current_tcb;
}


=======
#include "stm32f303.h"
#include <stdint.h>

extern uint32_t _task1_end;
uint32_t speed=0;
uint32_t d1=0;
uint32_t global_tick;
TCB _stcb[SIZE];
TCB *current_tcb;
TCB *next_tcb;

void tcbinit(void){
    
    // list of functions 
    void (*function_list[SIZE])(void *) = {
        my_task1,
        my_task2,
        my_task3,
        my_task4,
        my_task5,
        my_task6,
        my_task7,
        my_task8,
        my_task9,
        my_task10
    };

    for(int i = 0; i < SIZE; i++){
        
        // _stcb[i].sp = &(_task1_end)+ i*0x100 ;
        _stcb[i].context = 0;
        _stcb[i].function = function_list[i];
        _stcb[i].pid = i;
        _stcb[i].state = 0;
        _stcb[i].prio = 0xB;
        _stcb[i].flags = 0xC;
    }

}

void my_task1(void *ctx){
    (void)ctx;
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

void update_speed(void){
    speed = (TIM4->CNT-d1);
    d1=TIM4->CNT;
}

void my_task2(void *ctx){
    (void)ctx;
    while (1){
        ResetPins(GPIOA,0x20);
    }
}

void my_task3(void *ctx){
    (void)ctx;
    while(1){
        PinWrite(GPIOA, 0x20);
    } 
}

void my_task4(void *ctx){
    (void)ctx;
    while (1){
        ResetPins(GPIOA,0x20);
    }
}

void my_task5(void *ctx){
    (void)ctx;
    while(1){
        PinWrite(GPIOA, 0x20);
    } 
}

void my_task6(void *ctx){
    (void)ctx;
    while (1){
        ResetPins(GPIOA,0x20);
    }
}

void my_task7(void *ctx){
    (void)ctx;
    while(1){
        PinWrite(GPIOA, 0x20);
    } 
}

void my_task8(void *ctx){
    (void)ctx;
    while (1){
        ResetPins(GPIOA,0x20);
    }
}

void my_task9(void *ctx){
    (void)ctx;
    while(1){
        PinWrite(GPIOA, 0x20);
    } 
}

void my_task10(void *ctx){
    (void)ctx;
    while (1){
        ResetPins(GPIOA,0x20);
    }
}

TCB* taskscheduler(void){
    next_tcb++;
    if(next_tcb >= &_stcb[SIZE]){
        next_tcb = _stcb;
    }
    return next_tcb;
}
>>>>>>> Stashed changes
