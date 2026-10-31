#ifndef DIO_CFG_H
#define DIO_CFG_H
#include "Dio_Types.h"

#define DIO_PORT_A          ((Dio_PortType)0U)
#define DIO_PORT_B          ((Dio_PortType)1U)
#define DIO_CHANNEL_BUZZER  ((Dio_ChannelType)10U) /* PA10 */
#define DIO_CHANNEL_COUNT   ((Dio_ChannelType)32U)

#define DIO_MODULE_ID       ((uint16)120U)
#define DIO_INSTANCE_ID     ((uint8)0U)
#define DIO_SID_READ_CH     ((uint8)0x00U)
#define DIO_SID_WRITE_CH    ((uint8)0x01U)
#define DIO_SID_READ_PORT   ((uint8)0x02U)
#define DIO_SID_WRITE_PORT  ((uint8)0x03U)
#define DIO_SID_FLIP_CH     ((uint8)0x11U)
#define DIO_E_PARAM_INVALID_CHANNEL_ID ((uint8)0x0AU)
#define DIO_E_PARAM_INVALID_PORT_ID    ((uint8)0x14U)

#endif
