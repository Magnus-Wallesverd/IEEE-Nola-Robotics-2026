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

int expo(int num,int pow){
     for(int i =0; i < pow;i++){
	 num *= num;
     }
     return num;
	
}
int sin(int num){
     int ans =0;
     int det =1;
     for(int i =0; i<10; i++){
	 ans += 100*expo(num, i)/det ;
	 det *= i;
     }
}



