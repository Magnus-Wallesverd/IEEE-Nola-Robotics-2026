#include "stm32f303.h"

int twos_complement(int val){
	val = ~val + 1;
	return val;
}

uint8_t twos_complement_8(uint8_t val){
    val = ~val+1;
    return val;
}

int32_t int32(int16_t val){
    int32_t temp;
    if(val>>16){
        temp = val | 0xFFFF0000;
    }
    else{
        temp = val;
    }
    return temp;
}

void lcd_wait(void){
    for(volatile int i = 0; i < 6 * 64; i++);
}

static void setup(uint32_t PINS){
    PinWrite(GPIOA, E_PIN);
    PinWrite(GPIOB, (PINS<<OFFSET)&0xFF00);
    lcd_wait();
    ResetPins(GPIOA, E_PIN);
    ResetPins(GPIOB, DATA_PINS);
    lcd_wait();
}

void byte2LCD(char data, uint16_t ctrl_pins){
    PinWrite(GPIOA, ctrl_pins|E_PIN);
    PinWrite(GPIOB, data<<OFFSET);
    lcd_wait();
    ResetPins(GPIOA, E_PIN);
    ResetPins(GPIOA, ctrl_pins);
    ResetPins(GPIOB, data<<OFFSET);
}

static void putchar(char buffer){
    PinWrite(GPIOA, RS_E_PINS);
    PinWrite(GPIOB, buffer<<OFFSET);
    lcd_wait();
    ResetPins(GPIOA, E_PIN);
    ResetPins(GPIOA, RS_PIN);
    ResetPins(GPIOB, buffer<<OFFSET);
    lcd_wait();
}

// hex to char array
static void stringify(uint32_t num, char buffer[]){
    char temp = 0;

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

static void signed_stringify_8(uint8_t num, char buffer[]){
    char temp = 0;
    if(num>>7){
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
        lcd_wait();
        ResetPins(GPIOA, E_PIN);
        ResetPins(GPIOA, RS_PIN);
        ResetPins(GPIOB, buffer[i]<<OFFSET);
        lcd_wait();
    }
}

static void lcd_ddram_cmd(uint32_t addr){
    PinWrite(GPIOA, E_PIN);
    PinWrite(GPIOB, addr<<OFFSET);
    lcd_wait();
    ResetPins(GPIOA, E_PIN);
    ResetPins(GPIOB, addr<<OFFSET);
    lcd_wait();
}

void lcd_init(void){
    
    // port setup
    SetPinOutput(GPIOB, DATA_PINS);     //PB15-8  D7-0
    SetPinOutput(GPIOA, RS_RW_E_PINS);  //PA10-8  RS,RW,E
                                 
    SetOutputType(GPIOA, RW_PIN, 1);

    SetPinPD(GPIOA, PA8|PA10); 
    SetPinPD(GPIOB, DATA_PINS);
    SetPinPU(GPIOA, RW_PIN);
    
    // initialization
    setup((E_PIN|FUNC_SET));
    setup((E_PIN|DISP_SET));
    setup((E_PIN|CLR_LCD));
    for(volatile int i = 0; i < 1818*16; i++);
}

void lcd_print(void* args){
    (void)args;
    char entry_0[] = {"Wait  "};
    char entry_1[] = {"Step  "};
    char entry_2[] = {"Rotate"};
    char entry_3[] = {"Left  "};
    char entry_4[] = {"Right "};
    char entry_5[] = {"Cntr  "};
    char* lu_table[] = {entry_1,entry_2,entry_3,entry_4};
    char buffer_1[BYTE_BUFFER] = {0};
    char buffer_2[BYTE_BUFFER] = {0};
    char buffer_3[BYTE_BUFFER] = {0};
    char buffer_4[BYTE_BUFFER] = {0};
    char buffer_5[BYTE_BUFFER] = {0};
    char buffer_6[BYTE_BUFFER] = {0};
    
    // move_cursor(1, 0);
    // print(entry_5);

    uint32_t t0 = 0;
    uint32_t t1 = 0;
    
    while(1){
        t0 = get_global_tick();
        while((t0-t1) > REFRESH_RATE){

            move_cursor(0, 0);
            // stringify((i2c_rx_buffer[HEADING_MSB] << 8 | i2c_rx_buffer[HEADING_LSB]), buffer_1);
            stringify(ToF_Distance, buffer_1);
            move_cursor(0, 7);
            stringify(bno_heading>>4, buffer_2);
            // move_cursor(1, 7);
            // stringify(TIM1->CCR1, buffer_3);
            // move_cursor(1, 0);
            // stringify(measure_h, buffer_4);
            move_cursor(2, 0 );
            stringify(TIM2->CNT, buffer_5);
            move_cursor(3, 0 );
            stringify(TIM3->CNT, buffer_6);
            move_cursor(2, 7 );
            stringify(TIM4->CNT, buffer_3);
            move_cursor(3, 7 );
            stringify(TIM8->CNT, buffer_4);

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
