#include "stm32f303.h"
#include <stdint.h>

#define QUEUE_SIZE 2 
#define POOL_SIZE 2

work_item_t task_pool[POOL_SIZE];

// TODO 
// __attribute__((section(".task_queue")))
work_item_t* task_queue[SIZE];

work_item_t** front = &task_queue[0];
work_item_t** end   = &task_queue[0];
uint32_t task_queue_count  = 0;


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
    // item gets set first
    work_item_t* item = *front;
    if(task_queue_count == 0){
        return (void*)0;
    }else if(front<=&task_queue[SIZE-1]){
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

// initializes tasks into work items into a work pool
void taskinit(void){
    
    // tasks table
    task_func_t function_table[POOL_SIZE] = {
        blink_led,
        blink_led,
    };
    
    // load a function pointers into a work item array
    for(int i = 0; i < POOL_SIZE; i++){
        task_pool[i].function = function_table[i];
        task_pool[i].args = (void*)0;
    }
    
    // need to enqueue the address of the task
    for(int i = 0; i < QUEUE_SIZE; i++){
        enqueue(&task_pool[i]);
    }
}
