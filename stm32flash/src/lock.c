#include "stm32f303.h"

void block(void){
    current_tcb->state = BLOCKED;
    yield();
}

void unblock(TCB* tcb){
    tcb->state = READY;
    enqueue(ready_queue_ptr, tcb);
}

void idle_task(void* args){
    (void) args;
    while(1);
}
