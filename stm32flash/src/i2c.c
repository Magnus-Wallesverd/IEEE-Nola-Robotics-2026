/* TODO 
 * add a callback for device registry
 * proper I2C_Init
 * DMA
 */
#include "i2c.h"
#include "rcc.h"
#include "gpio.h"
#include <stdint.h>
#include "timx.h"
int rx_i = 0;
int bno[] = {0x3d, 0xC, 0x1A};
int bno_flag = 0;
int tx_i = 0;
int ovf17 =0;
uint8_t flag = 0;
void TIM1_TRG_TIM17_IRQHandler(void){
    ovf17++;
    TIM17->SR = 0;
    flag = 1;
}
void I2C1_EV_IRQHandler(void){
    if (!flag){
        return;
    }
    if(I2C1->ISR & TXIS){
        if(bno_flag){
            I2C1->TXDR = bno[2];
        } else {
            I2C1->TXDR = bno[tx_i++];
        }
        return;
    }

    if(I2C1->ISR & TC){
        I2C1->CR2 = (1 << 25)|(NBYTES << 16)|READ|(BNO055 << 1);
        I2C1->CR2 |= START;
        return;
    }
    
    if(I2C1->ISR & RXNE){
        buf[rx_i++] = I2C1->RXDR;
        return;
    }

    if(I2C1->ISR & STOPF){
        I2C1->ICR |=STOPCF;
        rx_i = 0;
        tx_i = 0;
        flag = 0;
        return;
    }
}

void I2C_Init(I2C_TypeDef* I2Cx, uint8_t mode){

    switch(mode){
        case 0:
            SetPinAlternate(GPIOB,0xC0);            // Set pins 6 & 7 to AF mode I2c
            AlternateFunctionSet(GPIOB,0xC0,4);      // set pins PB 6&7 to AF4
            SetOutputType(GPIOB, 0xC0, 1);
            SetOutputSpeed(GPIOB,0xC0, 1);

            I2Cx->CR1 &= ~(1<<0);
            I2Cx->TIMINGR = 0x10420F13;
            I2Cx->ICR = 0x3F38;
            I2Cx->CR1 |= TXIE|RXIE|TCIE|STOPIE;
            I2Cx->CR1 |= (1<<0);
            
        // case 1:
        // case 2:
    }
}

void sensor_clock_init(){

    RCC->APB2ENR |= 1 <<18;    // Enable TIM 17

    //TIM1 PWM

    //GPIO Control pins PB 1,2
    
    TIM17->DIER  |= 1;
    TIM17->CCMR1 |= 0x68;
    TIM17->PSC   |= 99;
    TIM17->ARR    = 7999;
    TIM17->CCR1  |= 4000;
    TIM17->CCER  |= 1;
    TIM17->BDTR  |= 1<<15;       // Main Output enable
    TIM17->CR1   |= 0b10000001;
}

void I2C_Write(I2C_TypeDef* I2Cx, uint8_t nbytes){
    int reg[] = {0x3D,0x07};
    I2Cx->CR2 = (1 << 25)|(nbytes << 16)|WRITE|(BNO055 << 1);
    I2Cx->CR2 &= ~(1<<25);
    I2Cx->CR2 &= ~(1 << 10);
    I2Cx->CR2 |= START;
    while (!(I2Cx->ISR & (1 << 1)));
    I2Cx->TXDR = reg[0];
    while (!(I2Cx->ISR & (1 << 6)));
    I2Cx->CR2 = (1 << 25)|(nbytes << 16)|WRITE|(BNO055 << 1);
    I2Cx->CR2 |= START;
    while (!(I2Cx->ISR & (1 << 1)));
    I2Cx->TXDR = reg[1];
    while (!(I2Cx->ISR & (1 << 6)));
}

void I2C_Read(I2C_TypeDef* I2Cx, uint8_t slave_addr, uint8_t nbytes){
    I2Cx->ICR = 0x3F38;
    I2Cx->CR2 = (1 << 25)|(nbytes << 16)|(1 << 10)|(slave_addr << 1);
    I2Cx->CR2 |= START;   //start
    while(!(I2Cx->ISR & (1<<2)));
    buf[0] = I2Cx->RXDR;
    while(!(I2Cx->ISR & (1<<5)));
}

// void I2C_Write_Read(I2C_TypeDef* I2Cx, uint8_t slave_addr, uint8_t nbytes, uint8_t reg){
//     // I2C_Write(I2Cx, slave_addr, nbytes, reg);
//     I2C_Read(I2Cx, slave_addr, nbytes);
// }

void Sensor_Read(I2C_TypeDef* I2Cx){
    I2Cx->CR2 = (1 << 16)|(BNO055 << 1);
    I2Cx->CR2 &= ~(1<<25);
    I2Cx->CR2 &= ~(1 << 10);
    I2Cx->CR2 |= START;   //start
}

void Sensor_Write(I2C_TypeDef* I2Cx){
    I2Cx->CR2 = (2 << 16)|(BNO055 << 1);
    I2Cx->CR2 &= ~(1<<25);
    I2Cx->CR2 &= ~(1 << 10);
    I2Cx->CR2 |= START;   //start
}

void Sensor_Read_Wrapper(void* args){
    (void) args;
    bno_flag = 1;
    while(1){
        Sensor_Read(I2C1);
        for(int i = 0; i < 0x7FFF;i++);
    }
}

void Sensor_Write_Wrapper(void* args){
    (void) args;
    Sensor_Write(I2C1);
}
