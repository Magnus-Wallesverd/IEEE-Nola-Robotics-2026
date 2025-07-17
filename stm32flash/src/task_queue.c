#include "stm32f303.h"
#include <stdint.h>

queue_t task_queue;
queue_t* task_queue_ptr = &task_queue;
work_item_t task_pool[TASK_QUEUE_SIZE];
void* task_queue_array[TASK_QUEUE_SIZE];

const func_t fn_table[FUNC_POOL_SIZE] = {
    lcd_print
};

void task_queue_init(queue_t* q){
    q->count = 0;
    q->array = task_queue_array;
    q->size  = TASK_QUEUE_SIZE;
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
