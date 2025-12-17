#ifndef LOCK_H
#define LOCK_H

#include "stm32f303.h"
#include <stdint.h>

extern uint32_t lock(uint32_t* flag);
extern void unlock(uint32_t* flag);
extern void yield(void);

#endif // !LOCK_H

