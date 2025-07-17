#include "stm32f303.h"

void lcd_print(void* args){
    (void)args;
    unsigned char str1[] = "hello world 1";
    unsigned char str2[] = "hello world 2";
 
    PinWrite(GPIOC,0x1c0|0x14);
    for(volatile int i = 0; i < 60; i++);
    ResetPins(GPIOC, 1<<8);
    for(volatile int i = 0; i < 60; i++);
    ResetPins(GPIOC, 0xc0|0x14);
    for(volatile int i = 0; i < 60; i++);
    
    for(int i = 0; str1[i] != '\0'; i++){
        PinWrite(GPIOC, 0x500 + str1[i]);
        for(volatile int i = 0; i < 6; i++);
        ResetPins(GPIOC, 1<<8);
        ResetPins(GPIOC, 0x500 + str1[i]);
        for(volatile int i = 0; i < 6; i++);
    }

    // for(int i = 0; str2[i] != '\0'; i++){
    //     PinWrite(GPIOC, 0x500 + str2[i]);
    //     for(volatile int i = 0; i < 6; i++);
    //     ResetPins(GPIOC, 1<<8);
    //     ResetPins(GPIOC, 0x500 + str2[i]);
    //     for(volatile int i = 0; i < 6; i++);
    // }
}

void lcd_init(void){

    SetPinOutput(GPIOC, 0x7FF);
    SetOutputType(GPIOC,1<<9,1);
    SetPinPD(GPIOC, 0X5FF);
    SetPinPU(GPIOC,1<<9);
    
    //PinWrite(GPIOC,0x38);
        
    // function block
    PinWrite(GPIOC, 0x138);
    for(volatile int i = 0; i < 909; i++);
    ResetPins(GPIOC, 1<<8);
    for(volatile int i = 0; i < 909; i++);
    ResetPins(GPIOC, 0xFF);
    for(volatile int i = 0; i < 909; i++);

    // display block
    PinWrite(GPIOC, 0x10F);
    for(volatile int i = 0; i < 909; i++);
    ResetPins(GPIOC, 1<<8);
    for(volatile int i = 0; i < 909; i++);
    ResetPins(GPIOC, 0xFF);
    for(volatile int i = 0; i < 909; i++);
    
    // clear block
    PinWrite(GPIOC, 0x101);
    for(volatile int i = 0; i < 1818; i++);
    ResetPins(GPIOC, 1<<8);
    for(volatile int i = 0; i < 1818; i++);
    ResetPins(GPIOC, 0xFF);
    for(volatile int i = 0; i < 909; i++);  
}
