#include <stdint.h>
// then a pointer will just fill up the whole block with struct variables
#ifndef DATA_H
#define DATA_H
typedef struct{
volatile uint32_t PWM;    //x00 
volatile uint32_t SPEED;   //x04
} Thread_TypeDef;


// instance name of struct ((reference struct pointer *) desired address)
#define Threads ((Thread_TypeDef *) 0x20002000)

#endif

