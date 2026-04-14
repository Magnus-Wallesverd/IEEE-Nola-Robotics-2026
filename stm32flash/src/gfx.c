#include <stdint.h>
#include "gfx.h"
#include "stmath.h"

#define WIDTH 8 
#define HEIGHT 12 

uint16_t dst0[WIDTH*HEIGHT*16];
uint16_t dst1[WIDTH*HEIGHT*16];
uint16_t* dst2[] = {dst0, dst1};

void bmp_size_up(bmp_t* bmp, uint8_t scale){
    uint16_t* src = bmp->addr;
    uint8_t n = bmp->size;
    static uint8_t buf = 0;

    uint8_t offset = scale*WIDTH;
    
    int row_offset = WIDTH*pow(scale,2);
    int temp = 0;

    for(int i = 0; i < n; ++i){
        for(int j = 0; j < scale; ++j){
            for(int k = 0; k < scale; ++k){
                dst2[buf][((i%WIDTH)*scale)+(j*offset)+temp+k] = src[i];
            }
        }
        if((i+1)%WIDTH==0){
            temp += row_offset;
        }
    }
    bmp->addr = dst2[buf];
    buf ^= 1;
}
