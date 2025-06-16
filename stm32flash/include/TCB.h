#ifndef TCB_H
#define TCB_H

#include <stdint.h>

typedef struct{
    uint32_t *sp;   // this should store the sp address? confirm 
    uint8_t  pid;
    uint8_t  state;
    uint8_t  prio;
    uint8_t  flags;

}TCB;

#endif // !TCB_H
#define TCB_H

