#ifndef I2C_TYPES_H
#define I2C_TYPES_H
#include "Std_Types.h"
typedef struct { 
    uint32 busClockHz; 
    uint32 timeout; 
} I2c_ConfigType;
#endif
