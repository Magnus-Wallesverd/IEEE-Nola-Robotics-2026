#ifndef SEM_H
#define SEM_H

#include <stdint.h>
#include "tcb.h"

typedef struct {
    uint32_t flag;
    void* item;
} sem_t;

// add external objects here
// probably the array of semaphores
// probably individual special instances

extern sem_t sem_blocked[TCB_ARRAY_SIZE];

void signal(sem_t* s);

uint32_t wait(sem_t* s);

#endif // !SEM_H

