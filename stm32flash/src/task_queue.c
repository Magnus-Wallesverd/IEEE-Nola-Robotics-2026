#include <stdint.h>
#include "queue.h"
#include "tcb.h"
#include "usart.h"
#include "lcd.h"
#include "i2c.h"
#include "motors.h"
#include "parser.h"
#include "statemachine.h"

queue_t task_queue;
queue_t* task_queue_ptr = &task_queue;

queue_t transport_queue;
queue_t* transport_queue_ptr = &transport_queue;

queue_t motor_queue;
queue_t* motor_queue_ptr = &motor_queue;

queue_t ready_queue;
queue_t* ready_queue_ptr = &ready_queue;

queue_t priority_queue;
queue_t* priority_queue_ptr = &priority_queue;

work_item_t task_pool[TASK_QUEUE_SIZE];
transport_item_t transport_pool[TRANSPORT_QUEUE_SIZE];
motor_item_t motor_pool[MOTOR_QUEUE_SIZE];

void* task_queue_array[TASK_QUEUE_SIZE];
void* transport_queue_array[TRANSPORT_QUEUE_SIZE];
void* motor_queue_array[MOTOR_QUEUE_SIZE];
void* ready_queue_array[TASK_QUEUE_SIZE];
void* priority_queue_array[TASK_QUEUE_SIZE];

// should start thinking of easier ways to get functions in here
const func_t fn_table[] = {
    Sensor_Read_Wrapper,
    sm_main,
    motor_handler,
    // usart_state_update,
    // transport_handler,
    // // lcd_print,
    // usart_begin,

    // relative_pos
};

// const motor_t motor_table[] = {
//     step3,
//     rotate3,
//     step3,
//     rotate3,
//     step3,
//     rotate3,
//     step3,
//     rotate3,
// };
//
// const motor_payload motor_payload_table[] = {
//     {.args = 60,  .speed = 2},
//     {.args = 1440,  .speed = 1},
//     {.args = 60,  .speed = 2},
//     {.args = 2880,  .speed = 1},
//     {.args = 60,  .speed = 2},
//     {.args = 0xffc44320,  .speed = 1},
//     {.args = 60,  .speed = 2},
//     {.args = 0,  .speed = 1},
//
// };

void task_queue_init(void){
    task_queue_ptr->array = task_queue_array;
    task_queue_ptr->count = 0;
    task_queue_ptr->size  = TASK_QUEUE_SIZE;
    task_queue_ptr->front = task_queue_ptr->array;
    task_queue_ptr->end   = task_queue_ptr->array;
    
    // load a function pointers into a work item array
    for(unsigned int i = 0; i < sizeof(fn_table)/4; i++){
        task_pool[i].fn = fn_table[i];
        task_pool[i].args = (void*)0;
    }

    // need to enqueue the address of the task
    for(unsigned int i = 0; i < sizeof(fn_table)/4; i++){
        enqueue(task_queue_ptr, &task_pool[i]);
    }

}

void transport_queue_init(void){

    transport_queue_ptr->array = transport_queue_array;
    transport_queue_ptr->count = 0;
    transport_queue_ptr->size  = TRANSPORT_QUEUE_SIZE;
    transport_queue_ptr->front = transport_queue_ptr->array;
    transport_queue_ptr->end   = transport_queue_ptr->array;
    
}

void motor_queue_init(void){

    motor_queue_ptr->array = motor_queue_array;
    motor_queue_ptr->count = 0;
    motor_queue_ptr->size  = MOTOR_QUEUE_SIZE;
    motor_queue_ptr->front = motor_queue_ptr->array;
    motor_queue_ptr->end   = motor_queue_ptr->array;

    // load a function pointers into a work item array
    // for(unsigned int i = 0; i < sizeof(motor_table)/4; i++){
    //     motor_pool[i].fn = motor_table[i];
    //     motor_pool[i].args = (void*)(&motor_payload_table[i]);
    // }
    //
    // // need to enqueue the address of the task
    // for(unsigned int i = 0; i < sizeof(motor_table)/4; i++){
    //     enqueue(motor_queue_ptr, &motor_pool[i]);
    // }
    
}
void mass_enqueue(motor_t motor_table[], motor_payload motor_payload_table[], int size){

    for(int i = 0; i < size; i++){
        motor_pool[i].fn = motor_table[i];
        motor_pool[i].args = (void*)(&motor_payload_table[i]);
    }

    // need to enqueue the address of the task
    for(int i = 0; i < size; i++){
        enqueue(motor_queue_ptr, &motor_pool[i]);
    }

}

void ready_queue_init(void){
    ready_queue_ptr->count = 0;
    ready_queue_ptr->array = ready_queue_array;
    ready_queue_ptr->size  = TASK_QUEUE_SIZE;
    ready_queue_ptr->front = ready_queue_ptr->array;
    ready_queue_ptr->end   = ready_queue_ptr->array;
    
    for(unsigned int i = 1; i < sizeof(fn_table)/4; i++){
        enqueue(ready_queue_ptr, &_stcb[i]);
    }
}

void priority_queue_init(void){
    priority_queue_ptr->count = 0;
    priority_queue_ptr->array = priority_queue_array;
    priority_queue_ptr->size  = TASK_QUEUE_SIZE;
    priority_queue_ptr->front = priority_queue_ptr->array;
    priority_queue_ptr->end   = priority_queue_ptr->array;
}
