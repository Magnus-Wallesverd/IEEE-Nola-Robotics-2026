#include "stm32f303.h"

void lcdprint(void){
    unsigned char str1[] = "Hallo World!";
    for(int i = 0; str1[i] != '\0'; i++){
        PinWrite(GPIOC, 0x500 + str1[i]);
        for(volatile int i = 0; i < 6; i++);
        ResetPins(GPIOC, 1<<8);
        ResetPins(GPIOC, 0x500 + str1[i]);
        for(volatile int i = 0; i < 6; i++);
    }
}

void lcdinit(void){

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
    
    lcdprint();
    /*    
    while(1){

        PinWrite(GPIOC, 0x101); //load
        for(volatile int i = 0; i < 454; i++);
        ResetPins(GPIOC, 1<<8); //execute clr
        for(volatile int i = 0; i < 227; i++);
        SetPinInput(GPIOC, 1<<7); // read D7
        for(volatile int i = 0; i < 227; i++);
        PinWrite(GPIOC,0x300); // RW, E High
        for(volatile int i = 0; i < 1818; i++);
        SetPinOutput(GPIOC,1<<7);
        ResetPins(GPIOC, 0x7FF);
        for(volatile int i = 0; i < 909; i++);
    }
    */  
        
}
