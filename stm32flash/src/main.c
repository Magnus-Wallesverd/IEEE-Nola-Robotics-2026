/* Test Main */
#include "stm32f303.h"
#include <math.h>

void put_char(char c){
    asm(
            "mov r0, #0x03\n"
            "mov r1, %[msg]\n"
            "bkpt #0xAB\n"
            : : [msg] "r" (&c) : "r0", "r1"
            );
}
void printf(const char *s){
    while(*s){
        put_char(*(s++));
    }
}
void printn(int num,int len){

    for(int i = 0; i <len ; i++){
        char str = ((num>>(len-i-1))& 1)+'0';
        put_char(str);
        if(i%4 == 3){
            put_char(' ');
        }
    }
     
}
void main(void){
//   int t = 0;
    int k =0;
    int u = 500;
    while(1){
        k= u-TIM3->CNT;
        TIM1->CCR1 = (TIM1->ARR)* fabs(2*atan(k/100)/3); 
    }
}
 
