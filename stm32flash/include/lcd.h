#ifndef LCD_H
#define LCD_H

#include <stdint.h>

#include "gpio.h"
#include "motors.h"

#define OFFSET          8
#define DATA_PINS       0xFF00
#define RS_PIN          PA9
#define RW_PIN          PA10
#define E_PIN           PA8
#define RS_E_PINS       E_PIN|RS_PIN
#define RS_RW_E_PINS    E_PIN|RS_PIN|RW_PIN
#define FUNC_SET        0x38
#define DISP_SET        0x0F
#define CLR_LCD         0x01
#define LINE1           0x80
#define LINE2           0xC0
#define LINE3           0x80
#define LINE4           0xC0
#define MAX_WIDTH       19
#define BUFFER_SIZE     5
#define NUM_BASE        0x30
#define REFRESH_RATE    20

enum lcd_mode{
    BIT_MODE_4 = 0,
    BIT_MODE_8
};

extern int16_t error2;

// void lcd_init(GPIO_TypeDef* dp, GPIO_TypeDef* cp, uint8_t pins, uint8_t bit_mode, uint8_t offset);

void lcd_init(void);

void lcd_print(void* args);

// move cursor (cartesian) 
void move_cursor(uint32_t x,uint32_t y);

#endif // !LCD_H
