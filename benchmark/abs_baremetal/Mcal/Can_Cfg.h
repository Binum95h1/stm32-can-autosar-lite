#ifndef CAN_CFG_H
#define CAN_CFG_H

#include "Can_Types.h"

#define CAN_MAIN_CONTROLLER          ((uint8)0U)
#define CAN_MAIN_HOH_ABS_TX          ((Can_HwHandleType)0U)

#define CAN_MODULE_ID                ((uint16)80U)
#define CAN_INSTANCE_ID              ((uint8)0U)
#define CAN_SID_INIT                 ((uint8)0x00U)
#define CAN_SID_WRITE                ((uint8)0x06U)
#define CAN_SID_SET_CONTROLLER_MODE  ((uint8)0x03U)
#define CAN_SID_GET_CONTROLLER_MODE  ((uint8)0x12U)
#define CAN_SID_GET_ERROR_STATE      ((uint8)0x11U)
#define CAN_SID_DEINIT               ((uint8)0x10U)

#define CAN_E_PARAM_POINTER          ((uint8)0x01U)
#define CAN_E_PARAM_CONTROLLER       ((uint8)0x02U)
#define CAN_E_UNINIT                 ((uint8)0x05U)
#define CAN_E_TRANSITION             ((uint8)0x06U)
#define CAN_E_PARAM_DATA_LENGTH      ((uint8)0x07U)

extern const Can_ConfigType Can_Config;

#endif /* CAN_CFG_H */
