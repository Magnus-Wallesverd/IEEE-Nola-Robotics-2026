// testing purposes.
// philosphy should be...
// "every .c file should #include the header for the API it uses"
//
// this prevents long chains of headers from being used
// also cleaner to read in the C file and is one more file
//

#ifndef STM32F303_H
#define STM32F303_H

#include "tcb.h"
#include "lock.h"
#include "queue.h"
#include "backend.h"
#include "gpio.h"
#include "timx.h"
#include "rcc.h"
#include "exti.h"
#include "syscfg.h"
#include "lcd.h"
#include "i2c.h"
#include "motors.h"
#include "spi.h"
#include "dma.h"

#endif
