#ifndef PDUR_H
#define PDUR_H

#include "Std_Types.h"
#include "PduR_Types.h"
#include "PduR_Cfg.h"

extern const PduR_ConfigType PduR_Config;

void PduR_Init(const PduR_ConfigType *ConfigPtr);
Std_ReturnType PduR_ComTransmit(PduIdType TxPduId, const PduInfoType *PduInfoPtr);
void PduR_MainFunction(void);

#endif
