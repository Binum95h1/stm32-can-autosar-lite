#ifndef MCAL_I2C_H
#define MCAL_I2C_H
#include <stdint.h>
#include "Std_Types.h"
void I2c_Init(void);
uint8_t I2c_MasterTransmit(uint8_t addr7, const uint8_t *data, uint8_t len);
#endif
