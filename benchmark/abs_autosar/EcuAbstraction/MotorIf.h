#ifndef MOTORIF_H
#define MOTORIF_H

#include "Std_Types.h"
#include "MotorIf_Types.h"
#include "MotorIf_Cfg.h"

void MotorIf_Init(void);
void MotorIf_SetPwm(uint16 ccr1, uint16 ccr2);
uint16 MotorIf_GetPwmPeriod(void);
uint16 MotorIf_GetEncoderCount1(void);
uint16 MotorIf_GetEncoderCount2(void);
void MotorIf_SetDirection1(uint8 in1, uint8 in2);
void MotorIf_SetDirection2(uint8 in1, uint8 in2);

#endif 
