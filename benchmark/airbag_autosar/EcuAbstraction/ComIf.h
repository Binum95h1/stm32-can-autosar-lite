#ifndef COMIF_H
#define COMIF_H
#include "Std_Types.h"
#include "ComIf_Types.h"
#include "ComIf_Cfg.h"
#include "Can_Types.h"

void ComIf_Init(void);
Std_ReturnType ComIf_SendDeployFlag(boolean deploy_flag);
Std_ReturnType ComIf_SendCanDiagStatus(const Can_DebugStatusType *status);
uint32 ComIf_GetDeployTxOkCount(void);
uint32 ComIf_GetDeployTxFailCount(void);
uint8 ComIf_GetDeploySeqCounter(void);

#endif
