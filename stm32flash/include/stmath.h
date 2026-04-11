#ifndef STMATH_H
#define STMATH_H

#include <stdint.h>

int16_t twos_compl16(int16_t value);
int32_t twos_compl32(int32_t value);

int16_t abs(int16_t value);
int32_t abs_32(int32_t value);

int32_t pow(uint32_t num, uint8_t times);

#endif // !MATH_H
//
//
//
//
//
//
//

extern int16_t sin[360];
extern int16_t cos[360];
