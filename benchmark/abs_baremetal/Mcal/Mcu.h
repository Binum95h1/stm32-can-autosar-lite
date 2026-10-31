#ifndef MCU_H
#define MCU_H
#include <stdint.h>
void Mcu_Init(void);
uint32_t Mcu_GetTick(void);
void Mcu_Delay(uint32_t ms);
#endif

