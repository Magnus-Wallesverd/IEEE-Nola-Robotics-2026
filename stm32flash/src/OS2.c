#include "OS2.h"
#include "stm32f303.h"
#include "gpio.h"
#include "tcb.h"
#include <stdint.h>
//process 0 stored stackpointer
//process +1 is status
//process +2 is function address
//process 14 to + 30 Context Frame
//process 32 + 64  datas
static int i =0;
int32_t dummy =0;
int32_t dummy2 =0;
int32_t dummy3 =0;
Manager* deb  =((Manager*)(&_process1 + 32));
threads* tasklist[5] = {
    task1,
    task2,
    task3,
    task4,
    task5,
};
threads* current_t = task5;

void exit_return(){
    current_t->fn &= 0;
    current_t->status = 0;
    unready();
    yield();
}
void set_priority(uint8_t num){
    current_t->status &= ~(0b1111 << 8); //clear priority
    current_t->status |= ( num << 8); //set priority
}


void cpu_time(uint8_t time_ms){
    current_t->alloc = (time_ms << 8) | time_ms;
}

void q_up(void (*task_func)(void*) ){
    for(int j =0; j<=4 ; j++){
        if((uint32_t)task_func ==  tasklist[j]->fn){ //check for already exist task
            return;
        }
    }
    for(int i =0;i <=4 ;i++){
        uint32_t status = tasklist[i]->status;
        if(status == 0){
            tasklist[i]->sp = (uint32_t)&tasklist[i]->context[4]; //set stackpointer
            tasklist[i]->context[19] = 0x01000000; //xPSC
            tasklist[i]->context[18] = (uint32_t)task_func;  //PC
            tasklist[i]->context[17] = (uint32_t)(&exit_return);   //LR
            tasklist[i]->context[12] = (uint32_t)(&tasklist[i]->data[0]); //address to process in r0
            tasklist[i]->fn = (uint32_t)task_func;  // function address
            tasklist[i]->alloc = 2 | (2<<8);
            tasklist[i]->status = 1; // Change task status to 1

            mem->priorBit |= 1;
            mem->readylist[0] |= 1 <<i ;
            return;
        }
    }
}



void select_task() {  //contact switching function
    mem->r2 = 31 - __builtin_clz(mem->priorBit);   //look through priority bit
    while(!(mem->readylist[mem->r2])){   //check readylist not empty
        mem->priorBit &= ~(1 << mem->r2);
        mem->r2 = 31 - __builtin_clz(mem->priorBit);
    }

    mem->r0 = 31 -( __builtin_clz(mem->readylist[mem->r2])) ;  //grab the task number
    if(current_t == tasklist[i]){ //  round robin same priority tasks
        mem->r1 = mem->readylist[mem->r2] & ~(1 << i);
        mem->r1 = 31 - ( __builtin_clz(mem->r1));
        if(mem->r1 != 255){
            i = mem->r1;
            current_t = tasklist[mem->r1];
            return;
        }

    }
    dummy = mem->r0;
    current_t = tasklist[mem->r0];
    i = mem->r0;
}

void wait(uint32_t ticks){
    mem->block[i] = get_global_tick() + ticks*2;
    tasklist[i]->status = 2; //change status to block
    unready();
    yield();
}

void unready(void){
    mem->r3 = tasklist[i]->status >>8; //r3 hold priority number
    mem->readylist[mem->r3] &= (1 << i);
    
}


__attribute__((naked)) void task_manager(void* args){
    q_up(&global_pos);
    /*q_up(&dht_rapper);*/
    NVIC->ISER0 |= 0xFFFFFFFF;
    /*NVIC->ISER1 |= 0xFFFFFFFF;*/
    /*NVIC->ISER2 |= 0xFFFFFFFF;*/
    /*NVIC->ISER3 |= 0xFFFFFFFF;*/
    /*NVIC->ISER4 |= 0xFFFFFFFF;*/
    /*NVIC->ISER5 |= 0xFFFFFFFF;*/
    /*NVIC->ISER6 |= 0xFFFFFFFF;*/
    /*NVIC->ISER7 |= 0xFFFF;*/

    mem->priorBit &= ~(1 << 31);
    while(1){
        mem->Ticks = get_global_tick();
        for(int j =0; j <=4; j++){
            if(mem->block[j]<= mem->Ticks && (tasklist[j]->status == 2)){ //unblock logic
                tasklist[j]->status = 1;
            }

        }

        yield();

    }
}
