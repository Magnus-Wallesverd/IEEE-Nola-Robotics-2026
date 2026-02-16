#include "lock.h"
#include "tcb.h"
#include "semaphore.h"
#include "queue.h"


void block(void){
    current_tcb->state = BLOCKED;
    yield();
}

//cant enqueue here
void unblock(TCB* tcb){
    tcb->flags++;
    kernel_unblock_counter++;
}

void kernel_unblock(sem_t* s){
    if(wait(s) == 1){
        enqueue(ready_queue_ptr, s->item);
    }
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
