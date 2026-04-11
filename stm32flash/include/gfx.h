#ifndef GFX_H
#define GFX_H

#include <stdint.h>

typedef struct{
    uint16_t* addr;
    uint32_t  size;
    uint8_t   minc;
}bmp_t;

void bmp_size_up(bmp_t* bmp, uint8_t scale);

#endif // !GFX_H
