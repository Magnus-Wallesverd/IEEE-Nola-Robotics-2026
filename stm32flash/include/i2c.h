//this is the i2c.h header file 
//


#ifndef I2C_H
#define I2C_H

#include <stdint.h>
// #include 

#define I2C_BUFFER_SIZE 4
#define STANDARD_MODE 0
#define FAST_MODE 1
#define FAST_MODE_PLUS 2
#define BNO055 0x28

#define READ    1<<10
#define START   1<<13
#define NBYTES  4

#define TXIE    1<<1 
#define RXIE    1<<2 
#define STOPIE  1<<5
#define TCIE    1<<6

#define TXIS    1<<1 
#define RXNE    1<<2 
#define STOPF   1<<5
#define TC      1<<6

#define STOPCF  1<<5

typedef struct {
    volatile uint32_t CR1;       // 0x00
    volatile uint32_t CR2;       // 0x04
    volatile uint32_t OAR1;      // 0x08
    volatile uint32_t OAR2;      // 0x0C
    volatile uint32_t TIMINGR;   // 0x10
    volatile uint32_t TIMEOUTR;  // 0x14
    volatile uint32_t ISR;       // 0x18
    volatile uint32_t ICR;       // 0x1C
    volatile uint32_t PECR;      // 0x20
    volatile uint32_t RXDR;      // 0x24
    volatile uint32_t TXDR;      // 0x28
} I2C_TypeDef;

// Peripheral base addresses (APB1)
#define I2C1 ((I2C_TypeDef *)0x40005400)
#define I2C2 ((I2C_TypeDef *)0x40005800)

extern uint8_t buf[I2C_BUFFER_SIZE];

// I2C HAL function prototypes

void I2C_Init(I2C_TypeDef* I2Cx, uint8_t mode);

void I2C_Write(I2C_TypeDef* I2Cx, uint8_t slave_addr, uint8_t nbytes, uint8_t reg);

void I2C_Read(I2C_TypeDef* I2Cx, uint8_t slave_addr, uint8_t nbytes);

void I2C_Write_Read(I2C_TypeDef* I2Cx, uint8_t slave_addr, uint8_t nbytes, uint8_t reg);

void Sensor_Read(I2C_TypeDef* I2Cx);

#endif // I2C_H

