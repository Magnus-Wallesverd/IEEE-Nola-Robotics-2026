#include "ST7796S.h"
#include "lock.h"
#include "queue.h"
#include "gfx.h"
#include "testfont.h"

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
    BLUE_GREY,
    NAVY,     
    OFF_WHITE,
    WHITE,    
    BLACK    
};

bmp_t fill = {
    &colors[0],
    0,
    MINC_OFF
};

bmp_t font = {
    num_1,
    FONT_AREA,
    MINC_EN
};

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

void lcd_wait(void){
    while(ST7796S.SPIx->CR2 & SPI_TXEIE);
    while(ST7796S.SPIx->SR & SPI_BSY);
}

void lcd_start(void){
    lcd_wait();
    CS_LOW();
}

void lcd_write_cmd(uint8_t cmd){

    lcd_wait();
     
    lcd_cmd = cmd;
    SPI_Dev_p->tx_buf = &lcd_cmd;
    SPI_Dev_p->tx_len = 1;

    DC_CMD();
    ST7796S.SPIx->CR2 |= SPI_TXEIE;
}

void lcd_dma_stream(bmp_t* bmp){

    lcd_wait();

    uint16_t dummy;

    change_datasize(ST7796S.SPIx, PIXEL_WIDTH);

    DC_DATA();

    while(bmp->size){
        lcd_wait();
        if(bmp->size > MAX_TRANSFER){
            DMA_TXRX_Transfer(ST7796S.CH_RX, ST7796S.CH_TX, MSIZE_HW, PSIZE_HW, MINC_OFF, bmp->minc, MAX_TRANSFER, &dummy, bmp->addr);
            bmp->size -= MAX_TRANSFER;
        } else {
            DMA_TXRX_Transfer(ST7796S.CH_RX, ST7796S.CH_TX, MSIZE_HW, PSIZE_HW, MINC_OFF, bmp->minc, bmp->size, &dummy, bmp->addr);
            bmp->size -= bmp->size;
        }
    }
    change_datasize(ST7796S.SPIx, CMD_WIDTH);
}

void lcd_write_data(uint8_t* pdata, uint16_t tx_len){

    lcd_wait();
    
    ST7796S.tx_buf = pdata;
    ST7796S.tx_len = tx_len;

    DC_DATA();

    ST7796S.SPIx->CR2 |= SPI_TXEIE;
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

void set_rect_color(uint16_t color, bmp_t bmp){
    
}

void lcd_draw_rect(uint16_t Xs, uint16_t Xe, uint16_t Ys, uint16_t Ye, bmp_t* bmp){
    
    uint32_t rect_area = (Xe-Xs+1)*(Ye-Ys+1);
    bmp->size = rect_area;

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
    lcd_dma_stream(bmp);

    lcd_end();
}

void lcd_demo(void* args){
    (void) args;
    /*fill.addr = &colors[2];*/
    /*lcd_draw_rect(0, (WIDTH) - 1, 0, (HEIGHT)-1, &fill);*/

    fill.addr = &colors[5];
    lcd_draw_rect(0, (WIDTH) - 1, 0, (HEIGHT)-1, &fill);

    lcd_draw_rect(0, (8) - 1, 0, (12)-1, &font);

    /*lcd_draw_rect(UI_COL_W1-1, UI_COL_W1-1 + BORDER/2, 0, (HEIGHT)-1, &colors[14]);*/

    /*lcd_draw_rect(UI_COL_W2-1, UI_COL_W2-1 + BORDER/2, 0, (HEIGHT)-1, &colors[14]);*/

    /*lcd_draw_rect(0, UI_COL_W1-1, UI_ROW_H1-1, UI_ROW_H1-1+BORDER/2, &colors[14]);*/

    /*lcd_draw_rect(0, UI_COL_W1-1, UI_ROW_H2-1, UI_ROW_H2-1+BORDER/2, &colors[14]);*/

    /*lcd_draw_rect(UI_COL_W1-1+BORDER/2, WIDTH-1, HEIGHT/2-1, HEIGHT/2-1+BORDER/2, &colors[14]);*/
}
