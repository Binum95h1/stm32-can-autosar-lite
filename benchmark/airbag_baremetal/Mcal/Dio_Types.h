#ifndef DIO_TYPES_H
#define DIO_TYPES_H
#include "Std_Types.h"

typedef uint16 Dio_ChannelType;
typedef uint16 Dio_PortType;
typedef uint16 Dio_PortLevelType;
typedef uint8 Dio_LevelType;

typedef struct {
    Dio_PortType port;
    Dio_PortLevelType mask;
    uint8 offset;
} Dio_ChannelGroupType;

#endif
