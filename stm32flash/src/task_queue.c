#include "stm32f303.h"
#include <stdint.h>

#define QUEUE_SIZE 2 
#define POOL_SIZE 2

work_item_t task_pool[POOL_SIZE];
work_item_t* task_queue[SIZE];
work_item_t** front = &task_queue[0];
work_item_t** end   = &task_queue[0];
uint32_t task_queue_count  = 0;

// initializes tasks into work items into a work pool
void taskinit(void){
    
    // tasks table
    task_func_t function_table[POOL_SIZE] = {
        blink_led,
        lcdprint
    };
    
    // load a function pointers into a work item array
    for(int i = 0; i < POOL_SIZE; i++){
        task_pool[i].func = function_table[i];
        task_pool[i].args = (void*)0;
    }
    
    // need to enqueue the address of the task
    for(int i = 0; i < QUEUE_SIZE; i++){
        enqueue(&task_pool[i]);
    }
}

// TODO check if im returning the right type
void enqueue(work_item_t *task){
    if(task_queue_count == SIZE){
        return;
    }else if(end <= &task_queue[SIZE-1]){
        *end = task;
        end++;
        task_queue_count++;
    } else {
        end = &task_queue[0];
        *end = task;
        end++;
        task_queue_count++;
    }
}

work_item_t* dequeue(void){
    work_item_t* item = front;
    if(task_queue_count == 0){
        return front;
    }else if(front<=&task_queue[SIZE]){
        *front = (void*)0;
        front++;
        task_queue_count--;
        return item;
    } else {
        front = &task_queue[0];
        *front = (void*)0;
        front++;
        task_queue_count--;
        return item;
    }
}
