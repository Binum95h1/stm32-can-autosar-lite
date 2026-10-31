#ifndef BARE_CAN_H
#define BARE_CAN_H

#include "Std_Types.h"
#include "Can_Types.h"

typedef struct {
    uint16 adcRaw;
    uint8 brake;
    uint8 direction;
    uint8 sequence;
    uint8 dlc;
} BareCan_MainCommandType;

void BareCan_Init(void);

void BareCan_RxIndication(
    uint16 canId,
    const uint8 *data,
    uint8 dlc
);

Std_ReturnType BareCan_ReadMainCommand(
    BareCan_MainCommandType *cmd
);

Std_ReturnType BareCan_SendAbsFeedback(
    uint8 wheel1Speed,
    uint8 wheel2Speed,
    uint8 motorState,
    uint8 sequence
);

Std_ReturnType BareCan_SendCanDiagStatus(
    const Can_DebugStatusType *status
);

#endif
