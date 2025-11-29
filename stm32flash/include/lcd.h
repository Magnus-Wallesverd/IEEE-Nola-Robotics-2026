#ifndef LCD_H
#define LCD_H

#define RS_PIN      0x400
#define E_PIN       0x100
#define RS_E_PINS   0x500
#define FUNC_SET    0x38
#define DISP_SET    0x0F
#define CLR_LCD     0x01
#define LINE1       0x80
#define LINE2       0xC0
#define LINE3       0x80
#define LINE4       0xC0
#define MAX_WIDTH   19
#define BUFFER_SIZE 5
#define NUM_BASE    0x30
#define REFRESH_RATE 16
extern double speed;
void lcd_init(void);
void lcd_print(void* args);

// move cursor (cartesian) 
void move_cursor(uint32_t x,uint32_t y);

#endif // !LCD_H
