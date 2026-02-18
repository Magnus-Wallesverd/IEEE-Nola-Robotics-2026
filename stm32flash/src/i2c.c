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
#include "lock.h"
#include "tcb.h"

int ovf17 = 0;
int bno_ready = 0;

void TIM1_TRG_TIM17_IRQHandler(void){
    ovf17++;
    TIM17->SR = 0;
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
            I2Cx->CR1 |= TXIE|RXIE|TCIE|STOPIE|NACKIE;
            I2Cx->CR1 |= (1<<0);
            
        // case 1:
        // case 2:
    }
    // sensor_clock_init();
}

void sensor_clock_init(){

    RCC->APB2ENR |= 1 <<18;    // Enable TIM 17

    //TIM1 PWM

    //GPIO Control pins PB 1,2
    
    TIM17->DIER  |= 1;
    TIM17->CCMR1 |= 0x68;
    TIM17->PSC   |= 40;
    TIM17->ARR    = 7999;
    TIM17->CCR1  |= 4000;
    TIM17->CCER  |= 1;
    TIM17->BDTR  |= 1<<15;       // Main Output enable
    TIM17->CR1   |= 0b10000001;
}

void Sensor_Read(I2C_TypeDef* I2Cx){
    I2Cx->CR2 = (NBYTES << 16)|(BNO055 << 1);
    I2Cx->CR2 &= ~(1<<25);
    I2Cx->CR2 &= ~(1 << 10);
    I2Cx->CR2 |= START;   //start
}

void Sensor_Write(I2C_TypeDef* I2Cx){
    I2Cx->CR2 = (INIT_NBYTES << 16)|(BNO055 << 1);
    I2Cx->CR2 &= ~(1<<25);
    I2Cx->CR2 &= ~(1 << 10);
    I2Cx->CR2 |= START;   //start
}

void Sensor_Read_Wrapper(void* args){
    (void) args;
    I2C_Init(I2C1, 0);
    while(get_global_tick() < 400){
        yield();
    }
    while(!bno_ready){
        Sensor_Write(I2C1);
        yield();
    }
    bno_flag++;
    while(1){
        if(!(I2C1->ISR&BUSY)){
            Sensor_Read(I2C1);
        }
        yield();
    }
}

void Sensor_Write_Wrapper(void* args){
    (void) args;
    Sensor_Write(I2C1);
}
