#include "stmath.h"

int16_t twos_compl16(int16_t value){
    return ~(value)+1;
}

int16_t abs(int16_t value){
    if(value < 0){
        value = twos_compl16(value);
    }
    return value;
}
