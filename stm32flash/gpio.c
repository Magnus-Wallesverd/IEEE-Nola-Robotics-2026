/* abstraction for gpio */

/*
 * TODO take a pointer to an array of pins for multiple pin sets
 * TODO setup up a bit mask for any 16 bit registers
 *
 */

#include "gpio.h"

// WIP
// void SetMode(GPIO_TypeDef *port, char pin, char mode)

void SetPinInput(GPIO_TypeDef *port, char pin){}
void SetPinOutput(GPIO_TypeDef *port, char pin){}
void SetPinAlternate(GPIO_TypeDef *port, char pin){}
void SetPinAnalog(GPIO_TypeDef *port, char pin){}

