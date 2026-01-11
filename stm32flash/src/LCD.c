#include "stm32f303.h"

static void setup(uint32_t PINS){
    PinWrite(GPIOA, PINS & RS_RW_E_PINS);
    PinWrite(GPIOC, (PINS<<8)&0xFF00);
    for(volatile int i = 0; i < 6; i++);
    ResetPins(GPIOA, E_PIN);
    ResetPins(GPIOC, DATA_PINS);
    for(volatile int i = 0; i < 6; i++);
}

static void setup_8_bit(GPIO_TypeDef* dp,GPIO_TypeDef* cp, uint32_t data){

    PinWrite(dp, data);
    PinWrite(cp, E_PIN);
    for(volatile int i = 0; i < 6; i++);
    ResetPins(cp, E_PIN);
    ResetPins(dp, 0xFF);
    for(volatile int i = 0; i < 6; i++);
}

void lcd_init(GPIO_TypeDef* dp, GPIO_TypeDef* cp, uint8_t pins, uint8_t bit_mode, uint8_t offset){
    switch(bit_mode){
        case BIT_MODE_4:
            SetPinOutput(dp, pins << offset);
            SetOutputType(cp, RW_PIN, 1);
            SetPinPD(dp, pins << offset);
            SetPinPU(cp,RW_PIN);
            setup_4_bit(dp, cp, pins << offset);
            break;

        case BIT_MODE_8:
            // port init
            SetPinOutput(dp, pins << offset);
            SetOutputType(cp,RW_PIN,1);
            SetPinPD(dp, pins << offset);
            SetPinPU(cp,RW_PIN);
            
            // initialization
            setup_8_bit(dp, cp, FUNC_SET << offset);
            setup_8_bit(dp, cp, DISP_SET << offset);
            setup_8_bit(dp, cp, CLR_LCD << offset);
            for(volatile int i = 0; i < 1818; i++);
            break;
    }
}

void byte2LCD(char pins, char ctrl_pins){
    PinWrite(GPIOC, ctrl_pins|E_PIN|pins);
    for(volatile int i = 0; i < 6; i++);
	LCD_CP ^= E_PIN;
}

static void putchar(char buffer){
    PinWrite(GPIOA, RS_E_PINS);
    PinWrite(GPIOB, buffer<<8);
    for(volatile int i = 0; i < 6; i++);
    ResetPins(GPIOA, E_PIN);
    ResetPins(GPIOA, RS_PIN);
    ResetPins(GPIOB, buffer<<8);
    for(volatile int i = 0; i < 6; i++)i;
}

// hex to char array
static void stringify(uint32_t num, char buffer[]){
    char temp = 0;
    char i = 0;
    
    while(num != 0){
        temp = num % 10;
        num /= 10;
        buffer[BUFFER_SIZE-1-i] = temp + NUM_BASE;
        i++;
    }

    for(uint8_t i = 0;i<BUFFER_SIZE; i++){
        putchar(buffer[i]);
    }
}

static void print(char buffer[]){
    for(int i = 0; buffer[i] != '\0'; i++){
        PinWrite(GPIOA, RS_E_PINS);
        PinWrite(GPIOC, buffer[i]<<8);
        for(volatile int i = 0; i < 6; i++);
        ResetPins(GPIOA, E_PIN);
        ResetPins(GPIOA, RS_PIN);
        ResetPins(GPIOC, buffer[i]<<8);
        for(volatile int i = 0; i < 6; i++);
    }
}

static void lcd_ddram_cmd(uint32_t addr){
    PinWrite(GPIOA, E_PIN);
    PinWrite(GPIOC, addr<<8);
    for(volatile int i = 0; i < 6; i++);
    ResetPins(GPIOA, E_PIN);
    ResetPins(GPIOC, addr<<8);
    for(volatile int i = 0; i < 6; i++);
}

void lcd_init(void){
    
    // port setup
    SetPinOutput(GPIOC, DATA_PINS);     //PC15-8 for D7-0
    SetPinOutput(GPIOA, RS_RW_E_PINS);  //PA10-8 for RS,RW,E
                                 
    SetOutputType(GPIOA, RW_PIN, 1);

    SetPinPD(GPIOA, RS_E_PINS); 
    SetPinPD(GPIOC, DATA_PINS);
    SetPinPU(GPIOA, RW_PIN);
    
    // initialization
    setup((E_PIN|FUNC_SET));
    setup((E_PIN|DISP_SET));
    setup((E_PIN|CLR_LCD));
    for(volatile int i = 0; i < 1818; i++);
}

void lcd_print(void* args){
    (void)args;

    static char entry_1[] ={"i2c something"};
    static char entry_2[] ={"i2c something"};
    char buffer[BUFFER_SIZE] = {0};
    move_cursor(0,0);
    print(entry_1);
    move_cursor(1,0);
    print(entry_2);
    int n = 0;
    uint32_t t0 = 0;
    uint32_t t1 = 0;
    while(1){
        t0 = get_global_tick();
        while((t0-t1) > REFRESH_RATE){
            move_cursor(n,8);
            stringify(*(get_i2c_buffer()+n),buffer);
            t1=t0;
            n ^= 1;
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
