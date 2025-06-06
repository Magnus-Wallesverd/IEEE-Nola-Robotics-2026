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

    for(int j =0; j <len/4; j++){
        for(int i = 0; i <len ; i++){
            char str = ((num>>(len-i-1))& 1)+'0';
            put_char(str);
        }
        put_char(' ');
    } 
}
void main(void){
    printf("Hello OPenOCD!");

    /* Reset the RCC clock configuration to the default reset state ------------*/
    /* Set HSION bit */
    //RCC->CR |= (uint32_t)0x00000001;

    /* Reset CFGR register */
    //RCC->CFGR &= 0xF87FC00C;

    /* Reset HSEON, CSSON and PLLON bits */
    //RCC->CR &= (uint32_t)0xFEF6FFFF;

    /* Reset HSEBYP bit */
    //RCC->CR &= (uint32_t)0xFFFBFFFF;

    /* Reset PLLSRC, PLLXTPRE, PLLMUL and USBPRE bits */
    //RCC->CFGR &= (uint32_t)0xFF80FFFF;

    /* Reset PREDIV1[3:0] bits */
    //RCC->CFGR2 &= (uint32_t)0xFFFFFFF0;

    /* Reset USARTSW[1:0], I2CSW and TIMs bits */
    //RCC->CFGR3 &= (uint32_t)0xFF00FCCC;

    /* Disable all interrupts */
    //RCC->CIR = 0x00000000;
    // for(volatile int i = 0; i < 10000; i++);
    
    RCC->AHBENR |= 0x20000;
     /* RCC->APB1ENR |= 1;
     */
    SetPinOutput(GPIOA,0x21);
    /*AlternateFunctionSet(GPIOA,0x03,1);
    InitGenTIM(TIM2);
    */
    PinWrite(GPIOA,0x21);
    while(1){
        printn(0b0101);
        printf(" PortA\n");
    }
 }
