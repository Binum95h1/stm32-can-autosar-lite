#ifndef MCU_TYPES_H
#define MCU_TYPES_H
#include "Std_Types.h"

typedef uint8 Mcu_ClockType;
typedef uint8 Mcu_RawResetType;

typedef enum {
    MCU_PLL_UNLOCKED = 0,
    MCU_PLL_LOCKED
} Mcu_PllStatusType;

typedef enum {
    MCU_POWER_ON_RESET = 0,
    MCU_SW_RESET,
    MCU_RESET_UNDEFINED
} Mcu_ResetType;

typedef struct {
    uint32 sysClockHz;
    uint32 systickReload;
} Mcu_ConfigType;

#endif
