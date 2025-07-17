#ifndef QUEUE_H
#define QUEUE_H

#include "stm32f303.h"
#include <stdint.h>

#define TASK_QUEUE_SIZE 3 
#define FUNC_POOL_SIZE 3
#define MSG_POOL_SIZE 3
#define MSG_QUEUE_SIZE 3 


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

typedef struct {
    uint32_t tag;
    void*    data_ptr;
    uint32_t timestamp;
}lcd_item_t;

extern queue_t* task_queue_ptr;

void enqueue(queue_t* q, void* args);
void* dequeue(queue_t* q);
void task_queue_init(void);

extern uint32_t lock(uint32_t* flag);
extern void unlock(uint32_t* flag);
extern void yield(void);

#endif // !QUEUE_H
