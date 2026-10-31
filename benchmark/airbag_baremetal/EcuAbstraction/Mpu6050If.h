#ifndef MPU6050IF_H
#define MPU6050IF_H
#include "Std_Types.h"
#include "Mpu6050If_Types.h"
#include "Mpu6050If_Cfg.h"
Std_ReturnType Mpu6050If_Init(void);
Std_ReturnType Mpu6050If_ReadAxes(Mpu6050If_AxesType *axes);
uint8 Mpu6050If_GetWhoAmI(void);
sint32 Mpu6050If_RawToMg(sint16 raw);
#endif
