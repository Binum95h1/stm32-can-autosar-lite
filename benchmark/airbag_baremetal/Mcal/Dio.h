#ifndef DIO_H
#define DIO_H
#include "Std_Types.h"
#include "Dio_Types.h"
#include "Dio_Cfg.h"

void Dio_Init(const void *ConfigPtr);
Dio_LevelType Dio_ReadChannel(Dio_ChannelType ChannelId);
void Dio_WriteChannel(Dio_ChannelType ChannelId, Dio_LevelType Level);
Dio_PortLevelType Dio_ReadPort(Dio_PortType PortId);
void Dio_WritePort(Dio_PortType PortId, Dio_PortLevelType Level);
Dio_LevelType Dio_FlipChannel(Dio_ChannelType ChannelId);
void Dio_MaskedWritePort(Dio_PortType PortId, Dio_PortLevelType Level, Dio_PortLevelType Mask);
void Dio_InitPins(void);
void Dio_WriteBuzzer(uint8 level);

#endif
