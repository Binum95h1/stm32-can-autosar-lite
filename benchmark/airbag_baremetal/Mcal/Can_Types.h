#ifndef CAN_TYPES_H
#define CAN_TYPES_H

#include "Std_Types.h"

typedef uint8  Can_HwHandleType;
typedef uint16 Can_IdType;
typedef uint8  Can_ReturnType;

#define CAN_OK      ((Can_ReturnType)0U)
#define CAN_NOT_OK  ((Can_ReturnType)1U)
#define CAN_BUSY    ((Can_ReturnType)2U)

typedef enum {
    CAN_CS_STOPPED = 0U,
    CAN_CS_STARTED = 1U,
    CAN_CS_SLEEP   = 2U,
    CAN_CS_UNINIT  = 3U
} Can_ControllerStateType;

typedef enum {
    CAN_ERRORSTATE_ACTIVE = 0,
    CAN_ERRORSTATE_PASSIVE,
    CAN_ERRORSTATE_BUSOFF
} Can_ErrorStateType;

typedef struct {
    Can_IdType id;
    uint8 length;
    const uint8 *sdu;
    uint16 swPduHandle;
} Can_PduType;

typedef struct {
    uint32 baudrate;
    uint32 bitTimingRegisterValue;
} Can_ConfigType;

typedef struct {
    uint8 lec;
    uint8 tec;
    uint8 rec;
    uint8 ewgf;
    uint8 epvf;
    uint8 boff;
    Can_ErrorStateType errorState;
    Can_ControllerStateType controllerMode;
} Can_DebugStatusType;

#endif
