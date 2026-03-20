/* TODO 
 * add a callback for device registry
 * proper I2C_Init
 * DMA
 */
#include <stdint.h>
#include "i2c.h"
#include "rcc.h"
#include "gpio.h"
#include "timx.h"
#include "lock.h"
#include "tcb.h"
#include "backend.h"

int ovf17 = 0;
int bno_ready = 0;

uint8_t 

I2C_Dev  Dev;
I2C_Dev* Current_Dev = &Dev;

void TIM1_TRG_TIM17_IRQHandler(void){
    ovf17++;
    TIM17->SR = 0;
}


void I2C_Init(I2C_TypeDef* I2Cx, uint8_t mode){

    switch((uint32_t)I2Cx){
        case (uint32_t)I2C1:

            NVIC_IPR->IPR7 |= NVIC_IRQ_PRIORITY1 << 24;
            NVIC->ISER0 |= 1<<31;
            RCC->APB1ENR |= (1 << 21);

            SetPinAlternate(GPIOB,0xC0);            // Set pins 6 & 7 to AF mode I2c
            AlternateFunctionSet(GPIOB,0xC0,4);      // set pins PB 6&7 to AF4
            SetOutputType(GPIOB, 0xC0, 1);
            SetOutputSpeed(GPIOB,0xC0, 1);

            I2Cx->CR1 &= ~(1<<0);
            I2Cx->TIMINGR = 0x10420F13;
            I2Cx->ICR = 0x3F38;
            I2Cx->CR1 |= TXIE|RXIE|TCIE|STOPIE|NACKIE;
            I2Cx->CR1 |= (1<<0);
            break;

        case (uint32_t)I2C2:

            SetPinAlternate(GPIOF,0x3);            // Set pins 6 & 7 to AF mode I2c
            AlternateFunctionSet(GPIOF,0x3,4);      // set pins PB 6&7 to AF4
            SetOutputType(GPIOF, 0x3, 1);
            SetOutputSpeed(GPIOF,0x3, 1);

            I2Cx->CR1 &= ~(1<<0);
            I2Cx->TIMINGR = 0x10420F13;
            I2Cx->ICR = 0x3F38;
            I2Cx->CR1 |= TXIE|RXIE|TCIE|STOPIE|NACKIE;
            I2Cx->CR1 |= (1<<0);
            break;

    }
}

// void Sensor_Init(void){
//     BNO055_dev.addr = BNO055_ADDR;
//     VL53L1X_dev.addr = VL53L1X_ADDR;
// }

void Sensor_Read(I2C_TypeDef* I2Cx, uint16_t dev, uint8_t* tx_buf, uint16_t tx_len, uint8_t* rx_buf, uint16_t rx_len){

    Current_Dev->addr = dev;
    Current_Dev->Op = READ;
    Current_Dev->tx_buffer = tx_buf;
    Current_Dev->tx_len = tx_len;
    Current_Dev->rx_buffer = rx_buf;
    Current_Dev->rx_len = rx_len;

    I2Cx->CR2 = (Current_Dev->tx_len << 16)|(Current_Dev->addr << 1);
    I2Cx->CR2 &= AUTOEND_OFF;
    I2Cx->CR2 &= WRITE;
    I2Cx->CR2 |= START;   //start

}

void Sensor_Write(I2C_TypeDef* I2Cx, uint16_t dev, uint8_t* tx_buf, uint16_t tx_len){

    Current_Dev->addr = dev;
    Current_Dev->Op = WRITE;
    Current_Dev->tx_buffer = tx_buf;
    Current_Dev->tx_len = tx_len;
    Current_Dev->rx_buffer = rx_buf;
    Current_Dev->rx_len = rx_len;
    
    for(int i = 0; i < tx_len; i++){
        i2c_tx_buffer[i] = tx_buf[i];
    }

    I2Cx->CR2 = (Current_Dev->tx_len << 16)|(Current_Dev->addr << 1);
    I2Cx->CR2 &= AUTOEND_ON;
    I2Cx->CR2 &= WRITE;
    I2Cx->CR2 |= START;   //start
    
}

void Sensor_Read_Wrapper(void* args){
    (void) args;
    uint8_t tx_buf[2];
    uint8_t rx_buf[2];
    tx_buf[0] = VL53L1X_ID >> 8;
    tx_buf[1] = VL53L1X_ID & 0xFF;
    Sensor_Read(I2C1, VL53L1X,tx_buf, 2, rx_buf, 1);
    while(1);
}

void Sensor_Write_Wrapper(void* args){
    (void) args;
}
