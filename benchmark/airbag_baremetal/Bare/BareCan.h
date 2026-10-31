#ifndef BARECAN_H
#define BARECAN_H
#include "Std_Types.h"
#include "Can_Types.h"
#include "BareCan_Cfg.h"
void BareCan_Init(void);
Std_ReturnType BareCan_SendDeployFlag(boolean deploy_flag);
Std_ReturnType BareCan_SendCanDiagStatus(const Can_DebugStatusType *status);
uint32 BareCan_GetDeployTxOkCount(void);
uint32 BareCan_GetDeployTxFailCount(void);
uint8 BareCan_GetDeploySeqCounter(void);
#endif
