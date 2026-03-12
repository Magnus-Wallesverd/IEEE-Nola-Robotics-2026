#ifndef TCB_H
#define TCB_H

#include <stdint.h>
#include "queue.h"

#define TCB_ARRAY_SIZE 10
#define STACK_SIZE 1024
#define TASK_BLOCK (1024/sizeof(uint32_t))

// old tcb struct still in use 
// TODO redesign to save bytes
typedef struct {
    uint32_t *sp;
    uint32_t total_age;
    uint32_t queue_age;
    uint8_t  pid;
    uint8_t  state;
    uint8_t  prio;
    uint8_t  flags;
}TCB;

// states for threads
enum states{
    READY = 0,
    RUNNING,
    BLOCKED,
    IDLE
};

void worker_function(void);
void producer_function(sem_t* s);
void transport_producer_function(sem_t* s);

extern TCB _stcb[TCB_ARRAY_SIZE];

uint32_t get_global_tick(void);
uint8_t* get_i2c_buffer(void);

#endif // !TCB_H
