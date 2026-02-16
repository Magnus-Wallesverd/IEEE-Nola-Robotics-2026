#ifndef LOCK_H
#define LOCK_H

#include <stdint.h>
#include "queue.h"
#include "tcb.h"

extern TCB *current_tcb;
extern uint32_t lock(uint32_t* flag);
extern void unlock(uint32_t* flag);
extern void yield(void);
extern void yield_isr(void);

void block(void);

void unblock(TCB* tcb);

void flag_wait(TCB* tcb);

void flag_post(TCB* tcb);

#endif // !LOCK_H

