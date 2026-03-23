#ifndef I2C_H
#define I2C_H

#include <stdint.h>
// #include 

#define I2C_BUFFER_SIZE 4
#define STANDARD_MODE 0
#define FAST_MODE 1
#define FAST_MODE_PLUS 2

#define BNO055_ADDR 0x28
#define BNO055_RX_BUFFER_SIZE 64
#define BNO055_TX_BUFFER_SIZE 1
#define OPR_REG 0x3D
#define NDOF_MODE 0xC
#define SYS_CLK_STATUS 0x38
#define HEADING_REG 0x1A
#define HEADING_LSB 0
#define HEADING_MSB 1
#define HEADING_MAX_VALUE 5760


#define VL53L1X_ADDR 0x29
#define VL53L1X_ID   0x010F

#define AUTOEND_OFF  ~(1<<25)
#define AUTOEND_ON (1<<25)
#define WRITE   ~(1<<10)
#define READ    1<<10
#define START   1<<13
#define NBYTES  2
#define INIT_NBYTES  2

#define TXIE    1<<1 
#define RXIE    1<<2 
#define NACKIE  1<<4 
#define STOPIE  1<<5
#define TCIE    1<<6

#define TXIS    1<<1 
#define RXNE    1<<2 
#define NACKF   1<<4  
#define STOPF   1<<5
#define TC      1<<6
#define TCR     1<<7
#define BUSY    1<<15

#define NACKCF  1<<4  
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

typedef struct {
    uint8_t  addr; 
    uint16_t Op;
    uint8_t* tx_buffer;
    uint16_t tx_len;
    uint8_t* rx_buffer;
    uint16_t rx_len;
} I2C_Dev;

// Peripheral base addresses (APB1)
#define I2C1 ((I2C_TypeDef *)0x40005400)
#define I2C2 ((I2C_TypeDef *)0x40005800)

extern int bno_flag;
extern int bno_ready;

extern uint8_t i2c_rx_buffer[I2C_BUFFER_SIZE];
extern uint8_t i2c_tx_buffer[I2C_BUFFER_SIZE];
extern uint8_t bno_rx_buffer[BNO055_RX_BUFFER_SIZE];
extern uint8_t bno_tx_buffer[BNO055_TX_BUFFER_SIZE];

extern I2C_Dev*  BNO055;
extern I2C_Dev*  VL53L1X;

extern I2C_Dev* Current_Dev;

extern uint16_t ToF_Distance_p;
extern int16_t bno_heading;

// I2C HAL function prototypes

void I2C_Init(I2C_TypeDef* I2Cx, uint8_t mode);

void Sensor_Write(I2C_TypeDef* I2Cx, uint16_t dev, uint8_t* tx_buf, uint16_t tx_len);

void Sensor_Read(I2C_TypeDef* I2Cx, uint16_t dev, uint8_t* tx_buf, uint16_t tx_len, uint8_t* rx_buf, uint16_t rx_len);

void Sensor_Read_Wrapper(void* args);

void Sensor_Write_Wrapper(void* args);

void get_ToF_Distance(void* args);
 
void Sensor_Init(void);

void bno055_init(void);

void I2C_Wait(I2C_TypeDef* I2Cx);

uint8_t* get_i2c_buffer();
#endif // I2C_H
