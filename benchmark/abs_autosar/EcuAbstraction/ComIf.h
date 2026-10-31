#ifndef COMIF_H
#define COMIF_H

#include "Std_Types.h"
#include "ComIf_Types.h"
#include "ComIf_Cfg.h"
#include "Can_Types.h"
#include "PduR_Types.h"
#include "PduR_Cfg.h"

void ComIf_Init(void);
Std_ReturnType ComIf_SendAbsFeedback(uint8 wheel1Speed, uint8 wheel2Speed, uint8 motorState, uint8 sequence);
Std_ReturnType ComIf_SendCanDiagStatus(const Can_DebugStatusType *status);
Std_ReturnType ComIf_ReadMainCommand(ComIf_MainCommandType *cmd);
void ComIf_RxIndication(PduIdType RxPduId, const uint8 *SduDataPtr, uint8 SduLength);

#endif /* COMIF_H */
