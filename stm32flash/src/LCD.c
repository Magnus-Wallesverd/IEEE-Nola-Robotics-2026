#include "stm32f303.h"

void lcd_init(void){
    
    // setup
    SetPinOutput(GPIOC, 0x7FF);
    SetOutputType(GPIOC,1<<9,1);
    SetPinPD(GPIOC, 0X5FF);
    SetPinPU(GPIOC,1<<9);
    
    // function block
    PinWrite(GPIOC, (E_PIN|FUNC_SET));
    for(volatile int i = 0; i < 6; i++);
    ResetPins(GPIOC, E_PIN);
    for(volatile int i = 0; i < 6; i++);
    ResetPins(GPIOC, 0xFF);
    for(volatile int i = 0; i < 6; i++);

    // display block
    PinWrite(GPIOC, (E_PIN|DISP_SET));
    for(volatile int i = 0; i < 6; i++);
    ResetPins(GPIOC, E_PIN);
    for(volatile int i = 0; i < 6; i++);
    ResetPins(GPIOC, 0xFF);
    for(volatile int i = 0; i < 6; i++);
    
    // clear block
    PinWrite(GPIOC, (E_PIN|CLR_LCD));
    for(volatile int i = 0; i < 1818; i++);
    ResetPins(GPIOC, E_PIN);
    for(volatile int i = 0; i < 1818; i++);
    ResetPins(GPIOC, 0xFF);
    for(volatile int i = 0; i < 909; i++);  
}

void lcd_print(void* args){
    (void)args;
    static char entry_1[] ={"g_tick"};
    lcd_goto(0,0);

    for(int i = 0; entry_1[i] != '\0'; i++){
        PinWrite(GPIOC, RS_E_PINS + entry_1[i]);
        for(volatile int i = 0; i < 6; i++);
        ResetPins(GPIOC, E_PIN);
        ResetPins(GPIOC, RS_PIN + entry_1[i]);
        for(volatile int i = 0; i < 6; i++);
    }

    while(1){
        lcd_goto(0,8);
        PinWrite(GPIOC, RS_E_PINS + );
        for(volatile int i = 0; i < 6; i++);
        ResetPins(GPIOC, E_PIN);
        ResetPins(GPIOC, RS_PIN + entry_1[i]);
        for(volatile int i = 0; i < 6; i++);
    }
}

void lcd_goto(uint32_t x,uint32_t y){
    uint32_t temp;
    if(y>MAX_WIDTH){
        return;
    }
    switch(x){
        case 0:
            temp = (LINE1 | y);
            lcd_ddram_cmd(temp);
            break;
        case 1:
            temp = (LINE2 | y);
            lcd_ddram_cmd(temp);
            break;
        case 2:
            temp = (LINE3 | (y+MAX_WIDTH+1));
            lcd_ddram_cmd(temp);
            break;
        case 3:
            temp = (LINE4 | (y+MAX_WIDTH+1));
            lcd_ddram_cmd(temp);
            break;
    }
}

void lcd_ddram_cmd(uint32_t addr){
    PinWrite(GPIOC, (E_PIN|addr));
    for(volatile int i = 0; i < 6; i++);
    ResetPins(GPIOC, E_PIN);
    for(volatile int i = 0; i < 6; i++);
    ResetPins(GPIOC, addr);
    for(volatile int i = 0; i < 6; i++);
}
