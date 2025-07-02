#include "stm32f303.h"
#include <stdint.h>

#define QUEUE_SIZE 4

work_item_t task_queue[SIZE];
uint32_t *front = &task_queue[0];
uint32_t *end   = &task_queue[0];
uint32_t task_queue_count  = 0;

void enqueue(work_item_t task){
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

work_item_t* dequeue(){
    if(count == 0){
        return;
    }else if(front<=&task_queue[SIZE]){
        *front = 0;
        front++;
        task_queue_count--;
    } else {
        front = &task_queue[0];
        *front = 0;
        front++;
        task_queue_count--;
    }
}
