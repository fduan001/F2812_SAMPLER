#ifndef GPIO_H
#define GPIO_H

#define GPIO_DIR_OUTPUT    1
#define GPIO_DIR_INPUT     0

void GpioMateCardSet(UINT8 pin, UINT8 ops);
UINT8 GpioMateCardGet(UINT8 pin);

#endif
