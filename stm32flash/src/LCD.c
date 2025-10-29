#include "stm32f303.h"

static void setup(uint32_t PINS){

    PinWrite(GPIOC, PINS);
    for(volatile int i = 0; i < 6; i++);
    ResetPins(GPIOC, E_PIN);
    ResetPins(GPIOC, 0xFF);
    for(volatile int i = 0; i < 6; i++);
}

static void putchar(char buffer){
        PinWrite(GPIOC, RS_E_PINS + buffer);
        for(volatile int i = 0; i < 6; i++);
        ResetPins(GPIOC, E_PIN);
        ResetPins(GPIOC, RS_PIN + buffer);
        for(volatile int i = 0; i < 6; i++);
}

// hex to char array
static void stringify(uint32_t num, char buffer[]){
    char temp = 0;

    for(int i = 0;i<BUFFER_SIZE;i++){
        temp = num % 10;
        num /= 10;
        buffer[BUFFER_SIZE-1-i] = temp + NUM_BASE;
    }
    for(int i = 0;i<BUFFER_SIZE; i++){
        putchar(buffer[i]);
    }
}

static void print(char buffer[]){
    for(int i = 0; buffer[i] != '\0'; i++){
        PinWrite(GPIOC, RS_E_PINS + buffer[i]);
        for(volatile int i = 0; i < 6; i++);
        ResetPins(GPIOC, E_PIN);
        ResetPins(GPIOC, RS_PIN + buffer[i]);
        for(volatile int i = 0; i < 6; i++);
    }
}

static void lcd_ddram_cmd(uint32_t addr){
    PinWrite(GPIOC, (E_PIN|addr));
    for(volatile int i = 0; i < 6; i++);
    ResetPins(GPIOC, E_PIN);
    ResetPins(GPIOC, addr);
    for(volatile int i = 0; i < 6; i++);
}

void lcd_init(void){
    
    // port setup
    SetPinOutput(GPIOC, 0x7FF);
    SetOutputType(GPIOC,1<<9,1);
    SetPinPD(GPIOC, 0X5FF);
    SetPinPU(GPIOC,1<<9);
    
    // initialization
    setup((E_PIN|FUNC_SET));
    setup((E_PIN|DISP_SET));
    setup((E_PIN|CLR_LCD));
    for(volatile int i = 0; i < 1818; i++);
}

void lcd_print(void* args){
    (void)args;

    static char entry_1[] ={"speed"};
    static char entry_2[] ={"TIM4"};
    char buffer[BUFFER_SIZE] = {0};
    move_cursor(0,0);
    print(entry_1);
    move_cursor(1,0);
    print(entry_2);
    
    uint32_t x = 0;
    uint32_t t0 = 0;
    uint32_t t1 = 0;
    while(1){
        t0 = get_global_tick();
        while((t0-t1) > REFRESH_RATE){
            x=TIM4->CNT;
            move_cursor(0,6);
            stringify(abs(speed), buffer);
            move_cursor(1,6);
            stringify(x,buffer);
            // __asm volatile("BKPT #0");
            t1=t0;
        }
    }
}

void move_cursor(uint32_t x,uint32_t y){
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
