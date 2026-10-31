#ifndef I2C_H
#define I2C_H
#include "Std_Types.h"
#include "I2c_Types.h"
#include "I2c_Cfg.h"
void I2c_Init(const I2c_ConfigType *ConfigPtr);
Std_ReturnType I2c_WriteReg(uint8 addr7, uint8 reg, uint8 value);
Std_ReturnType I2c_ReadReg(uint8 addr7, uint8 reg, uint8 *value);
Std_ReturnType I2c_ReadRegMulti(uint8 addr7, uint8 reg, uint8 *data, uint8 len);
void I2C_Init_100kHz(void);
uint8 I2C_WriteReg(uint8 addr7, uint8 reg, uint8 value);
uint8 I2C_ReadReg(uint8 addr7, uint8 reg, uint8 *value);
uint8 I2C_ReadRegMulti(uint8 addr7, uint8 reg, uint8 *data, uint8 len);
#endif
