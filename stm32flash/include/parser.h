#ifndef PARSER_H
#define PARSER_H

#include <stdint.h>

typedef struct{
    uint32_t* src_i;
    uint32_t* dst_i;
    uint8_t  buffer_size;
    uint8_t  frame_size;
    uint8_t  ID;
    uint8_t* src;
    uint8_t* dst;
}parser_t;

void parse_array(void* args);

#endif // !PARSER_H
