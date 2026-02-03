#include "stm32f303.h"

int twos_complement(int val){
	val = ~val + 1;
	return val;
}

static void setup(uint32_t PINS){
    PinWrite(GPIOA, E_PIN);
    PinWrite(GPIOB, (PINS<<OFFSET)&0xFF00);
    for(volatile int i = 0; i < 6; i++);
    ResetPins(GPIOA, E_PIN);
    ResetPins(GPIOB, DATA_PINS);
    for(volatile int i = 0; i < 6; i++);
}

void byte2LCD(char data, uint16_t ctrl_pins){
    PinWrite(GPIOA, ctrl_pins|E_PIN);
    PinWrite(GPIOB, data<<OFFSET);
    for(volatile int i = 0; i < 6; i++);
    ResetPins(GPIOA, E_PIN);
    ResetPins(GPIOA, ctrl_pins);
    ResetPins(GPIOB, data<<OFFSET);
}

static void putchar(char buffer){
    PinWrite(GPIOA, RS_E_PINS);
    PinWrite(GPIOB, buffer<<OFFSET);
    for(volatile int i = 0; i < 6; i++);
    ResetPins(GPIOA, E_PIN);
    ResetPins(GPIOA, RS_PIN);
    ResetPins(GPIOB, buffer<<OFFSET);
    for(volatile int i = 0; i < 6; i++)i;
}

// hex to char array
static void stringify(uint32_t num, char buffer[]){
    char temp = 0;
    if(num>>31){
		byte2LCD('-', RS_PIN);
		num = twos_complement(num);
	} else {
		byte2LCD(' ', RS_PIN);
	}

    for(int i = 0;i<BUFFER_SIZE;i++){
        temp = num % 10;
        num /= 10;
        buffer[BUFFER_SIZE-1-i] = temp + NUM_BASE;
    }

    for(uint8_t i = 0;i<BUFFER_SIZE; i++){
        putchar(buffer[i]);
        buffer[i] = 0;
    }
}

// hex to char array
static void signed_stringify(uint32_t num, char buffer[]){
    char temp = 0;
    if(num>>31){
		byte2LCD('-', RS_PIN);
		num = twos_complement(num);
	} else {
		byte2LCD(' ', RS_PIN);
	}

    for(int i = 0;i<BUFFER_SIZE;i++){
        temp = num % 10;
        num /= 10;
        buffer[BUFFER_SIZE-1-i] = temp + NUM_BASE;
    }

    for(uint8_t i = 0;i<BUFFER_SIZE; i++){
        putchar(buffer[i]);
        buffer[i] = 0;
    }
}


static void print(char buffer[]){
    for(int i = 0; buffer[i] != '\0'; i++){
        PinWrite(GPIOA, RS_E_PINS);
        PinWrite(GPIOB, buffer[i]<<OFFSET);
        for(volatile int i = 0; i < 6; i++);
        ResetPins(GPIOA, E_PIN);
        ResetPins(GPIOA, RS_PIN);
        ResetPins(GPIOB, buffer[i]<<OFFSET);
        for(volatile int i = 0; i < 6; i++);
    }
}

static void lcd_ddram_cmd(uint32_t addr){
    PinWrite(GPIOA, E_PIN);
    PinWrite(GPIOB, addr<<OFFSET);
    for(volatile int i = 0; i < 6; i++);
    ResetPins(GPIOA, E_PIN);
    ResetPins(GPIOB, addr<<OFFSET);
    for(volatile int i = 0; i < 6; i++);
}

void lcd_init(void){
    
    // port setup
    SetPinOutput(GPIOB, DATA_PINS);     //PB15-8 for D7-0
    SetPinOutput(GPIOA, RS_RW_E_PINS);  //PA10-8 for RS,RW,E
                                 
    SetOutputType(GPIOA, RW_PIN, 1);

    SetPinPD(GPIOA, PA8|PA10); 
    SetPinPD(GPIOB, DATA_PINS);
    SetPinPU(GPIOA, RW_PIN);
    
    // initialization
    setup((E_PIN|FUNC_SET));
    setup((E_PIN|DISP_SET));
    setup((E_PIN|CLR_LCD));
    for(volatile int i = 0; i < 1818; i++);
}

void lcd_print(void* args){
    (void)args;
    char entry_1[] = {"error2: "};
    char buffer_1[BUFFER_SIZE] = {0};
    char buffer_2[BUFFER_SIZE] = {0};
    move_cursor(1,0);
    print(entry_1);
    uint32_t t0 = 0;
    uint32_t t1 = 0;
    while(1){
        t0 = get_global_tick();
        while((t0-t1) > REFRESH_RATE){
            move_cursor(0,0);
            signed_stringify((*(get_i2c_buffer()+1)<<8) + *(get_i2c_buffer()),buffer_1);
            move_cursor(1, 7);
            stringify(get_global_tick(), buffer_2);
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
