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
#include "vl53l1_platform.h"
#include "VL53L1X_api.h"
#include "VL53L1X_calibration.h"

int ovf17 = 0;
int bno_ready = 0;
int i2c_wait_counter = 0;

uint16_t ToF_Distance = 0;
uint8_t  ToF_offset_ADA = 29;
uint8_t  ToF_offset_PIM = 18;

int16_t offset = 0;


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
            RCC->APB1ENR1 |= (1 << 21);

            SetPinAlternate(GPIOA,1<<15);            // Set pins 6 & 7 to AF mode I2c
            AlternateFunctionSet(GPIOA,1<<15,4);      // set pins PB 6&7 to AF4
            SetPinAlternate(GPIOB,0x80);            // Set pins 6 & 7 to AF mode I2c
            AlternateFunctionSet(GPIOB,0x80,4);      // set pins PB 6&7 to AF4
            SetOutputType(GPIOA, 1<<15, 1);
            SetOutputSpeed(GPIOB,0x80, 1);

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

    Sensor_Init();
}

void I2C_Wait(I2C_TypeDef* I2Cx){
    i2c_wait_counter++;
    while(I2Cx->ISR & BUSY);
}

void Sensor_Init(void){
    uint16_t dev = VL53L1X_ADDR;
    uint8_t state = 0;
    int8_t status = VL53L1X_BootState(VL53L1X_ADDR, &state);
    int16_t calibrate_status = 0;
    I2C_Wait(I2C1);

    if(state){
        status = VL53L1X_SensorInit(dev);
        calibrate_status = VL53L1X_SetOffset(VL53L1X_ADDR, ToF_offset_ADA);
    } else {
        
    }
}

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

    I2Cx->CR2 = (Current_Dev->tx_len << 16)|(Current_Dev->addr << 1);
    I2Cx->CR2 |= AUTOEND_ON;
    I2Cx->CR2 &= WRITE;
    I2Cx->CR2 |= START;   //start
}

void get_ToF_Distance(void* args){
    (void) args;

    static uint8_t range_active = 0;
    uint8_t data_ready;

    if (!range_active) {
        VL53L1X_StartRanging(VL53L1X_ADDR);
        range_active = 1;
    }

    VL53L1X_CheckForDataReady(VL53L1X_ADDR, &data_ready);
    VL53L1X_GetDistance(VL53L1X_ADDR, &ToF_Distance);
    VL53L1X_ClearInterrupt(VL53L1X_ADDR);
}

void Sensor_Read_Wrapper(void* args){
    (void) args;
    while(1){
        get_ToF_Distance((void*) args);
        yield();

    }
}

void Sensor_Write_Wrapper(void* args){
    (void) args;
}
