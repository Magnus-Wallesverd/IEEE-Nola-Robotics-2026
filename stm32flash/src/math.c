#include <stdint.h>
#include "stm32f303.h"

int abs(int num){
    if(num<0){
        return num*(-1);
    }
    return num;
}



