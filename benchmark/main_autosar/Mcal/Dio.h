#ifndef MCAL_DIO_H
#define MCAL_DIO_H

#include "Std_Types.h"

typedef uint8 Dio_ChannelType;

#define DIO_CHANNEL_BRAKE_SW      ((Dio_ChannelType)0U)
#define DIO_CHANNEL_DIRECTION_SW  ((Dio_ChannelType)1U)

void Dio_Init(void);
uint8 Dio_ReadChannel(Dio_ChannelType ChannelId);

#endif /* MCAL_DIO_H */
