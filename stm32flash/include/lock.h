#ifndef LOCK_H
#include "stm32f303.h"
#include <stdint.h>


extern uint8_t lock(uint8_t* flag);
extern uint8_t unlock(uint8_t* flag);
extern void yield(void);

#endif // !LOCK_H

