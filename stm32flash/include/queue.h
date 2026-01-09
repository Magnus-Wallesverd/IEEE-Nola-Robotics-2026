#ifndef QUEUE_H
#define QUEUE_H

#include "stm32f303.h"
#include <stdint.h>

#define TASK_QUEUE_SIZE 2 
#define FUNC_POOL_SIZE 2
#define MSG_POOL_SIZE 2
#define MSG_QUEUE_SIZE 2 


typedef void (*func_t)(void* args);

typedef struct{
    uint32_t size;
    uint32_t count;
    void**   front;
    void**   end;
    void**   array;
}queue_t;

typedef struct {
    func_t fn;
    void* args;
}work_item_t;

extern queue_t* task_queue_ptr;

void enqueue(queue_t* q, void* args);
void* dequeue(queue_t* q);
void task_queue_init(void);

#endif // !QUEUE_H
