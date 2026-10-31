#ifndef COMIF_H
#define COMIF_H

#include "Std_Types.h"
#include "ComIf_Types.h"
#include "ComIf_Cfg.h"
#include "Can_Types.h"
#include "PduR_Types.h"
#include "PduR_Cfg.h"

void ComIf_Init(void);
Std_ReturnType ComIf_SendMainCommand(uint16 adcRaw, uint8 brake, uint8 direction, uint8 sequence);
Std_ReturnType ComIf_SendCanDiagStatus(const Can_DebugStatusType *status);
Std_ReturnType ComIf_ReadAbsFeedback(ComIf_AbsFeedbackType *feedback);
Std_ReturnType ComIf_ReadAirbagStatus(ComIf_AirbagStatusType *status);
void ComIf_RxIndication(PduIdType RxPduId, const uint8 *SduDataPtr, uint8 SduLength);

#endif /* COMIF_H */
