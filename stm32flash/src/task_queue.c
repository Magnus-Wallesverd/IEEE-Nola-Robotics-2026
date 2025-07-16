#include "stm32f303.h"
#include <stdint.h>

#define TASK_QUEUE_SIZE 3 
#define FUNC_POOL_SIZE 3

work_item_t task_pool[TASK_QUEUE_SIZE];

queue_t task_queue;
queue_t* task_queue_ptr = &task_queue;
void* task_queue_array[TASK_QUEUE_SIZE];

func_t fn_table[POOL_SIZE] = {
    blink_led,
    blink_led,
    blink_led
};

void task_queue_init(queue_t* q){
    q->array = task_queue_array;
    q->size  = TASK_QUEUE_SIZE;
    q->count = 0;
    q->front = q->array;
    q->end   = q->array;

    // load a function pointers into a work item array
    for(int i = 0; i < FUNC_POOL_SIZE; i++){
        task_pool[i].fn = fn_table[i];
        task_pool[i].args = (void*)0;
    }
    
    // need to enqueue the address of the task
    for(int i = 0; i < TASK_QUEUE_SIZE; i++){
        enqueue(q, &task_pool[i]);
    }
}
