#include <stdint.h>
#include "queue.h"
#include "tcb.h"
#include "usart.h"
#include "lcd.h"
#include "i2c.h"
#include "motors.h"
#include "parser.h"

queue_t task_queue;
queue_t* task_queue_ptr = &task_queue;

queue_t transport_queue;
queue_t* transport_queue_ptr = &transport_queue;

queue_t ready_queue;
queue_t* ready_queue_ptr = &ready_queue;

queue_t priority_queue;
queue_t* priority_queue_ptr = &priority_queue;

work_item_t task_pool[TASK_QUEUE_SIZE];
void* task_queue_array[TASK_QUEUE_SIZE];
void* transport_queue_array[TASK_QUEUE_SIZE];
void* ready_queue_array[TASK_QUEUE_SIZE];
void* priority_queue_array[TASK_QUEUE_SIZE];

// should start thinking of easier ways to get functions in here
const func_t fn_table[] = {
    lcd_print,
    Sensor_Read_Wrapper,
    usart_begin
};

void task_queue_init(void){
    task_queue_ptr->array = task_queue_array;
    task_queue_ptr->count = 0;
    task_queue_ptr->size  = TASK_QUEUE_SIZE;
    /*task_queue_ptr->max_time = 0;*/
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
    
    // use  to test
    // load a function pointers into a work item array
    //for(unsigned int i = 0; i < sizeof(fn_table)/4; i++){
    //    task_pool[i].fn = fn_table[i];
    //    task_pool[i].args = (void*)0;
    //}
    //
    // need to enqueue the address of the task
    //for(unsigned int i = 0; i < sizeof(fn_table)/4; i++){
    //    enqueue(task_queue_ptr, &task_pool[i]);
    //}

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
