#ifndef PARSER_H
#define PARSER_H

#include <stdint.h>

typedef struct{
    uint32_t parser_i;
    uint32_t ref_i;
    uint8_t* src;
    uint8_t* dst;
}parser_t;

void parse_array(void* args);

#endif // !PARSER_H
