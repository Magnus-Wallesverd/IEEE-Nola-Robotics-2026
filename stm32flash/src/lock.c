#include "stm32f303.h"

void block(void){
    current_tcb->state = BLOCKED;
    yield();
}

void unblock(TCB* tcb){
    tcb->state = READY;
    enqueue(ready_queue_ptr, tcb);
}

void flag_wait(TCB* tcb){
    tcb->flags--;
}

void flag_post(TCB* tcb){
    tcb->flags++;
}

void idle_task(void* args){
    (void) args;
    while(1);
}
