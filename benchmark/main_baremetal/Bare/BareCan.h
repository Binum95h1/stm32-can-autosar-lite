#ifndef BARECAN_H
#define BARECAN_H
#include "Std_Types.h"
#include "Can_Types.h"
typedef struct {
    uint8 wheel1Speed;
    uint8 wheel2Speed;
    uint8 motorState;
    uint8 sequence;
    uint8 dlc;
} BareCan_AbsFeedbackType;
typedef struct {
    uint8 deploy;
    uint8 sequence;
    uint8 reserved;
    uint8 dlc;
} BareCan_AirbagStatusType;
void BareCan_Init(void);
Std_ReturnType BareCan_SendMainCommand(uint16 adcRaw, uint8 brake, uint8 direction, uint8 sequence);
Std_ReturnType BareCan_SendCanDiagStatus(const Can_DebugStatusType *status);
Std_ReturnType BareCan_ReadAbsFeedback(BareCan_AbsFeedbackType *feedback);
Std_ReturnType BareCan_ReadAirbagStatus(BareCan_AirbagStatusType *status);
void BareCan_RxIndication(uint16 id, const uint8 *data, uint8 dlc);
#endif
