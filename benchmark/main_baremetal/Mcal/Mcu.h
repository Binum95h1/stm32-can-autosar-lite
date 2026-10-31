#ifndef MCAL_MCU_H
#define MCAL_MCU_H
#include <stdint.h>
#include "Std_Types.h"
void Mcu_InitClock(void);
void Mcu_InitSysTick(void);
void nvic_enable(uint32_t irq);
#endif
