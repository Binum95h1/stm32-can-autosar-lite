#ifndef RTE_AIRBAG_H
#define RTE_AIRBAG_H
#include "Std_Types.h"
#include "Mpu6050If.h"
#include "Can_Types.h"

Std_ReturnType Rte_Read_AirbagS_Acceleration(Mpu6050If_AxesType *axes);
Std_ReturnType Rte_Call_AirbagS_Buzzer_Set(boolean on);
Std_ReturnType Rte_Call_AirbagS_Com_SendDeployFlag(boolean flag);
Std_ReturnType Rte_Call_AirbagS_Com_SendCanDiagStatus(const Can_DebugStatusType *status);
Std_ReturnType Rte_Call_AirbagS_CanIf_GetDebugStatus(Can_DebugStatusType *status);
uint32 Rte_Call_AirbagS_Com_GetDeployTxOkCount(void);
uint32 Rte_Call_AirbagS_Com_GetDeployTxFailCount(void);
uint8 Rte_Call_AirbagS_Com_GetDeploySeqCounter(void);
uint32 Rte_Call_AirbagS_OsIf_GetTimeMs(void);
void Rte_Call_AirbagS_OsIf_DelayMs(uint32 ms);
void Rte_Call_AirbagS_Det_Log(const char *s);
void Rte_Call_AirbagS_Det_Logf(const char *fmt, ...);

/* Compatibility names from the previous manual RTE. */
#define Rte_Read_Acceleration(a)             Rte_Read_AirbagS_Acceleration(a)
#define Rte_Call_Buzzer_Set(on)              Rte_Call_AirbagS_Buzzer_Set(on)
#define Rte_Call_Com_SendDeployFlag(flag)    Rte_Call_AirbagS_Com_SendDeployFlag(flag)
#define Rte_Call_GetTimeMs()                 Rte_Call_AirbagS_OsIf_GetTimeMs()
#define Rte_Call_DelayMs(ms)                 Rte_Call_AirbagS_OsIf_DelayMs(ms)
#define Rte_Call_Log(s)                      Rte_Call_AirbagS_Det_Log(s)
#define Rte_Call_Logf(...)                   Rte_Call_AirbagS_Det_Logf(__VA_ARGS__)
#endif
