 /*
 * This is the main function for the develop branch
 * It will be left blank so any person wanting to branch from develop
 * Any main.c programs made on different branches should be left in its own repo
 * 
 * Again... Do not pull request to develop with a main.c that has changes until agreed on
 * */

#include "stm32f303.h"
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
    printf("Hello OPenOCD!"); 
    RCC->AHBENR |= 0x20000;
     /* RCC->APB1ENR |= 1;
     */
    SetPinOutput(GPIOA,0x21);
    /*AlternateFunctionSet(GPIOA,0x03,1);
    InitGenTIM(TIM2);
    */
    PinWrite(GPIOA,0x21);
    while(1){
        printn(GPIOA->IDR & 1 >>4,1);
        printf("PA4\n");
    }
 }
