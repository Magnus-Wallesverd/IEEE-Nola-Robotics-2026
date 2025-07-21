#ifndef QUEUE_H
#define QUEUE_H

#include "stm32f303.h"
#include <stdint.h>

#define TASK_QUEUE_SIZE 3 
#define FUNC_POOL_SIZE 3
#define MSG_POOL_SIZE 3
#define MSG_QUEUE_SIZE 3 
#define HIGH_QUEUE_MAX_TIME 10
#define MID_QUEUE_MAX_TIME 20
#define LOW_QUEUE_MAX_TIME 50


typedef void (*func_t)(void* args);

typedef struct{
    uint32_t size;
    uint32_t count;
    uint32_t max_time;
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
void demote_thread(TCB* tcb);
#endif // !QUEUE_H
