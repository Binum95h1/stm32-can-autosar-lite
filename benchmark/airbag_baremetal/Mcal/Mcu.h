#ifndef MCU_H
#define MCU_H
#include "Std_Types.h"
#include "Mcu_Types.h"

void Mcu_Init(const Mcu_ConfigType *ConfigPtr);
void Mcu_InitClock(void);
void Mcu_InitSysTick(void);
Mcu_PllStatusType Mcu_GetPllStatus(void);
Mcu_ResetType Mcu_GetResetReason(void);
Mcu_RawResetType Mcu_GetResetRawValue(void);
void Mcu_PerformReset(void);
uint32 Mcu_GetTickMs(void);
void SysTick_Handler(void);

#endif
