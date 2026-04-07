#ifndef ST7796S_H
#define ST7796S_H

#include <stdint.h>
#include "gpio.h"
#include "spi.h"

#define RST_HIGH() GPIOA->BSRR |= 1<<1
#define CS_LOW() GPIOA->BSRR |= 1<<(16+4)
#define CS_HIGH() GPIOA->BSRR |= 1<<4
#define DC_CMD() GPIOA->BSRR |= 1<<(16+6)
#define DC_DATA() GPIOA->BSRR |= 1<<6

#define DATA_BUFFER_SIZE 16

#define HEIGHT      320 
#define WIDTH       480
#define BORDER      9
#define UI_ROW_H1   160
#define UI_ROW_H2   267
#define PAD         120

#define SWRESET 0x01
#define SLPIN   0x10
#define SLPOUT  0x11
#define DISPON  0x29
#define CASET   0x2A
#define RASET   0x2B
#define RAMWR   0x2C
#define MADCTL  0x36
#define MV      (1<<5)
#define RGB     (1<<3)
#define COLMOD  0x3A
#define RGB565  0x55

#define E_BLUE      0xE318
#define CORAL       0xF3A9
#define MINT        0x67EC
#define SAGE        0x8DD0
#define SLATE       0x24FF
#define RED         0xF800
#define YELLOW      0xFFE0
#define ORANGE      0xFBE0
#define GREEN       0xE007
#define AMBER       0xDC40
#define BLACK       0x0000
#define OFF_WHITE   0xFFDD
#define WHITE       0xFFFF
#define D_GREEN     0x0902
#define NAVY        0x0844

void ST7796S_setup(SPI_TypeDef* SPIx);
void ST7796S_init(void* args);
void lcd_write_cmd(uint8_t cmd);
void lcd_write_data(uint8_t* pdata, uint16_t tx_len);
void lcd_write_data_byte(uint8_t data);
void lcd_start(void);
void lcd_end(void);
void lcd_demo(void* args);

#endif
