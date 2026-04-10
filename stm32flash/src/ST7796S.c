#include "ST7796S.h"
#include "lock.h"
#include "queue.h"

uint16_t colors[] = { 

    RED,
    ORANGE,   
    AMBER,    
    YELLOW,   
    SAGE,     
    GREEN,    
    D_GREEN,  
    MINT,     
    E_BLUE,   
    CORAL,    
    NAVY,     
    OFF_WHITE,
    WHITE,    
    SLATE,    
    BLACK,    
};

uint16_t red = 0xF800;

work_item_t fill_screen = {
    .fn = lcd_demo
};

uint8_t data_buf[4];

SPI_Dev_t ST7796S;
uint8_t lcd_cmd;
uint8_t lcd_data;

void ST7796S_setup(SPI_TypeDef* SPIx){
    ST7796S.SPIx  = SPIx;
    ST7796S.CH_RX = DMA1_CH2;
    ST7796S.CH_TX = DMA1_CH3;
    SPI_Dev_p     = &ST7796S;
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

void lcd_wait(void){
    while(ST7796S.SPIx->CR2 & SPI_TXEIE);
    while(ST7796S.SPIx->SR & SPI_BSY);
}

void lcd_write_cmd(uint8_t cmd){

    lcd_wait();
     
    lcd_cmd = cmd;
    SPI_Dev_p->tx_buf = &lcd_cmd;
    SPI_Dev_p->tx_len = 1;

    DC_CMD();
    ST7796S.SPIx->CR2 |= SPI_TXEIE;
}

void lcd_dma_stream(uint16_t* pdata, uint32_t tx_len, uint8_t minc_tx){
    lcd_wait();

    uint16_t dummy;

    change_datasize(ST7796S.SPIx, PIXEL_WIDTH);

    DC_DATA();

    while(tx_len){
        if(tx_len > MAX_TRANSFER){
            DMA_TXRX_Transfer(ST7796S.CH_RX, ST7796S.CH_TX, MSIZE_HW, PSIZE_HW, MINC_OFF, minc_tx, MAX_TRANSFER, &dummy, pdata);
            tx_len -= MAX_TRANSFER;
        } else {
            DMA_TXRX_Transfer(ST7796S.CH_RX, ST7796S.CH_TX, MSIZE_HW, PSIZE_HW, MINC_OFF, minc_tx, tx_len, &dummy, pdata);
            tx_len -= tx_len;
        }
    }
}

void lcd_write_data(uint8_t* pdata, uint16_t tx_len){

    lcd_wait();
    
    ST7796S.tx_buf = pdata;
    ST7796S.tx_len = tx_len;

    DC_DATA();

}

void lcd_write_data_byte(uint8_t data){
    lcd_wait();

    lcd_data = data;
    SPI_Dev_p->tx_buf = &lcd_data;
    SPI_Dev_p->tx_len = 1;

    DC_DATA();

    ST7796S.SPIx->CR2 |= SPI_TXEIE;
}

void lcd_end(void){
    lcd_wait();
    CS_HIGH();
}

void lcd_draw_rect(uint16_t Xs, uint16_t Xe, uint16_t Ys, uint16_t Ye, uint16_t* bmp){
    
    uint32_t rect_area = (Xe-Xs+1)*(Ye-Ys+1);

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
    lcd_dma_stream(bmp, rect_area, MINC_OFF);

    /*for(uint32_t i = 0; i < (uint32_t)((Xe-Xs+1)*(Ye-Ys+1)); i++){*/
    /*    lcd_write_data_byte(color >> 8);      */
    /*    lcd_write_data_byte(color & 0xFF);        */
    /*}*/

    lcd_end();
}

void lcd_demo(void* args){
    (void) args;

    /*lcd_draw_rect(0, (WIDTH) - 1, 0, (HEIGHT)-1, &red);*/
    lcd_draw_rect(100,  110, 0, 400, &colors[2]);
    // background
/*    lcd_draw_rect(0, (WIDTH) - 1, 0, (HEIGHT)-1, D_GREEN);*/
/**/
/*    // top left */
/*    lcd_draw_rect(BORDER, (WIDTH/2)- 1 - BORDER/2, BORDER, (UI_ROW_H1)-1-BORDER/2, OFF_WHITE);*/
/**/
/*    //top right*/
/*    lcd_draw_rect((WIDTH/2)-1+BORDER/2, (WIDTH) - 1-BORDER, BORDER, (UI_ROW_H1)-1-BORDER/2, OFF_WHITE);*/
/**/
/*    // middle left*/
/*    lcd_draw_rect(BORDER, (WIDTH/2)-1-BORDER/2, UI_ROW_H1-1+BORDER/2, UI_ROW_H2-1-BORDER/2, OFF_WHITE);*/
/**/
/*    // middle right*/
/*    lcd_draw_rect((WIDTH/2)-1+BORDER/2, (WIDTH)-1-BORDER, UI_ROW_H1-1+BORDER/2, UI_ROW_H2-1-BORDER/2, OFF_WHITE);*/
/**/
/*    //bottom center*/
/*    lcd_draw_rect(BORDER+PAD, (WIDTH)-1-BORDER-PAD, UI_ROW_H2-1+BORDER/2, HEIGHT - BORDER+1, OFF_WHITE);*/
/**/
}
