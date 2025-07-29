#include <stdint.h>
#include "stm32f303.h"

int abs(int num){
    if(num<0){
        return num*(-1);
    }
    return num;
}

int twos16Bit(uint16_t num){
    if(num >> 15){
        num = ~num;
        num++;
        return (int)num*(-1);
    }
    else{
        return (int)num;
    }
}


