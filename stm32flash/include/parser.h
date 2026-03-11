#ifndef PARSER_H
#define PARSER_H

#include <stdint.h>
#include "semaphore.h"

#define CAMERA_ID 0xAA

#define PARSER_BUFFER_SIZE 64

typedef struct{
    uint32_t* src_i;
    uint32_t* dst_i;
    uint8_t  buffer_size;
    uint8_t  frame_size;
    uint8_t  ID;
    uint8_t* src;
    uint8_t* dst;
    sem_t* work_sem_p;
}parser_t;

typedef struct {
    uint32_t ID;
    uint8_t* src;
    
}dispatcher_t;

void parse_array(void* args);

#endif // !PARSER_H
