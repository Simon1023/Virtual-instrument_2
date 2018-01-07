
#ifndef OV7725_INIT_H
#define OV7725_INIT_H

#include "stm32f7xx_hal.h"

void DCMI_OV7725_QVGASizeRaWSetup(I2C_HandleTypeDef *hi2c1);
void bayer2rgb(unsigned char *CAMBuffer);


#endif
