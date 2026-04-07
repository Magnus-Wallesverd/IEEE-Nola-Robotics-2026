#include "ST7796S.h"
#include "lock.h"
#include "queue.h"


work_item_t fill_screen = {
    .fn = lcd_demo
};

SPI_Dev_t ST7796S;
uint8_t lcd_cmd;
uint8_t lcd_data;
uint8_t data_buf[DATA_BUFFER_SIZE];

void ST7796S_setup(SPI_TypeDef* SPIx){
    ST7796S.SPIx = SPIx;
    SPI_Dev_p  = &ST7796S;
    SetPinOutput(GPIOA,PA1|PA4|PA6);
}

void ST7796S_init(void* args){
    (void)args;

    CS_HIGH();
    RST_HIGH();
    task_wait(150);
    
    lcd_start();
    lcd_write_cmd(SWRESET);
    lcd_end();
    task_wait(120);

    lcd_start();
    lcd_write_cmd(SLPOUT);
    lcd_end();
    task_wait(10);

    lcd_start();
    lcd_write_cmd(DISPON);
    lcd_end();
    task_wait(10);

    lcd_start();
    lcd_write_cmd(COLMOD);
    lcd_write_data_byte(RGB565);
    lcd_end();
    task_wait(10);

    lcd_start();
    lcd_write_cmd(MADCTL);
    lcd_write_data_byte(MV|RGB);
    lcd_end();

    enqueue(task_queue_ptr, &fill_screen);
}

void lcd_start(void){
    CS_LOW();
}

void lcd_write_cmd(uint8_t cmd){
    while(ST7796S.SPIx->CR2 & SPI_TXEIE);
    lcd_cmd = cmd;
    SPI_Dev_p->tx_buf = &lcd_cmd;
    SPI_Dev_p->tx_len = 1;

    DC_CMD();
    ST7796S.SPIx->CR2 |= SPI_TXEIE;

    /*while(!(ST7796S.SPIx->SR & SPI_TXE));*/
    /*SPI_Dev_p->SPIx->DR = cmd;*/
    /*volatile uint8_t spi_rx = *((volatile uint8_t*)&SPI1->DR);*/
    /*(void)spi_rx;*/
    while(ST7796S.SPIx->CR2 & SPI_TXEIE);
    while(ST7796S.SPIx->SR & SPI_BSY);
}

void lcd_write_data(uint8_t* pdata, uint16_t tx_len){
    while(ST7796S.SPIx->CR2 & SPI_TXEIE);
    ST7796S.tx_buf = pdata;
    ST7796S.tx_len = tx_len;

    DC_DATA();

    ST7796S.SPIx->CR2 |= SPI_TXEIE;

    while(ST7796S.SPIx->CR2 & SPI_TXEIE);
    while(ST7796S.SPIx->SR & SPI_BSY);
}

void lcd_write_data_byte(uint8_t data){
    while(ST7796S.SPIx->CR2 & SPI_TXEIE);
    lcd_data = data;
    SPI_Dev_p->tx_buf = &lcd_data;
    SPI_Dev_p->tx_len = 1;

    DC_DATA();
    ST7796S.SPIx->CR2 |= SPI_TXEIE;
    while(ST7796S.SPIx->CR2 & SPI_TXEIE);
    while(ST7796S.SPIx->SR & SPI_BSY);

}

void lcd_end(void){
    while(ST7796S.SPIx->CR2 & SPI_TXEIE);
    while(ST7796S.SPIx->SR & SPI_BSY);
    CS_HIGH();
}

void lcd_draw_rect(uint16_t Xs, uint16_t Xe, uint16_t Ys, uint16_t Ye, uint16_t color){
    
    data_buf[0] = Xs >> 8;
    data_buf[1] = Xs & 0xFF;
    data_buf[2] = Xe >> 8;
    data_buf[3] = Xe & 0xFF;
    
    lcd_start();
    lcd_write_cmd(CASET);
    lcd_write_data(data_buf, 4);

    data_buf[0] = Ys >> 8;
    data_buf[1] = Ys & 0xFF;
    data_buf[2] = Ye >> 8;
    data_buf[3] = Ye & 0xFF;

    lcd_write_cmd(RASET);
    lcd_write_data(data_buf, 4);
    
    lcd_write_cmd(RAMWR);
    for(uint32_t i = 0; i < (uint32_t)((Xe-Xs+1)*(Ye-Ys+1)); i++){
        lcd_write_data_byte(color >> 8);      
        lcd_write_data_byte(color & 0xFF);        
    }

    lcd_end();
}

void lcd_demo(void* args){
    (void) args;
    
    // background
    lcd_draw_rect(0, (WIDTH) - 1, 0, (HEIGHT)-1, D_GREEN);

    // top left 
    lcd_draw_rect(BORDER, (WIDTH/2)- 1 - BORDER/2, BORDER, (UI_ROW_H1)-1-BORDER/2, OFF_WHITE);

    //top right
    lcd_draw_rect((WIDTH/2)-1+BORDER/2, (WIDTH) - 1-BORDER, BORDER, (UI_ROW_H1)-1-BORDER/2, OFF_WHITE);

    // middle left
    lcd_draw_rect(BORDER, (WIDTH/2)-1-BORDER/2, UI_ROW_H1-1+BORDER/2, UI_ROW_H2-1-BORDER/2, OFF_WHITE);

    // middle right
    lcd_draw_rect((WIDTH/2)-1+BORDER/2, (WIDTH)-1-BORDER, UI_ROW_H1-1+BORDER/2, UI_ROW_H2-1-BORDER/2, OFF_WHITE);

    //bottom center
    lcd_draw_rect(BORDER+PAD, (WIDTH)-1-BORDER-PAD, UI_ROW_H2-1+BORDER/2, HEIGHT - BORDER+1, OFF_WHITE);

}
