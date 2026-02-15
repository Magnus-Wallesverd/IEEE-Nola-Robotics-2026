#include "stm32f303.h"

void block(void){
    current_tcb->state = BLOCKED;
    yield();
}

// where is this param coming from
void unblock(TCB* tcb){
    tcb->state = READY;
    enqueue(ready_queue_ptr, tcb);
}
