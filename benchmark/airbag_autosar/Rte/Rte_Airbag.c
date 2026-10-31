#include "Rte_Airbag.h"
#include "BuzzerIf.h"
#include "ComIf.h"
#include "CanIf.h"
#include "Can_Cfg.h"
#include "OsIf.h"
#include "Det.h"
#include <stdarg.h>
#include <stdio.h>

Std_ReturnType Rte_Read_AirbagS_Acceleration(Mpu6050If_AxesType *axes)
{
    return Mpu6050If_ReadAxes(axes);
}

Std_ReturnType Rte_Call_AirbagS_Buzzer_Set(boolean on)
{
    return BuzzerIf_Set(on);
}

Std_ReturnType Rte_Call_AirbagS_Com_SendDeployFlag(boolean flag)
{
    return ComIf_SendDeployFlag(flag);
}

Std_ReturnType Rte_Call_AirbagS_Com_SendCanDiagStatus(const Can_DebugStatusType *status)
{
    return ComIf_SendCanDiagStatus(status);
}

Std_ReturnType Rte_Call_AirbagS_CanIf_GetDebugStatus(Can_DebugStatusType *status)
{
    return CanIf_GetDebugStatus(CAN_MAIN_CONTROLLER, status);
}

uint32 Rte_Call_AirbagS_Com_GetDeployTxOkCount(void)
{
    return ComIf_GetDeployTxOkCount();
}

uint32 Rte_Call_AirbagS_Com_GetDeployTxFailCount(void)
{
    return ComIf_GetDeployTxFailCount();
}

uint8 Rte_Call_AirbagS_Com_GetDeploySeqCounter(void)
{
    return ComIf_GetDeploySeqCounter();
}

uint32 Rte_Call_AirbagS_OsIf_GetTimeMs(void)
{
    return OsIf_GetTimeMs();
}

void Rte_Call_AirbagS_OsIf_DelayMs(uint32 ms)
{
    OsIf_DelayMs(ms);
}

void Rte_Call_AirbagS_Det_Log(const char *s)
{
    Det_Print(s);
}

void Rte_Call_AirbagS_Det_Logf(const char *fmt,...)
{
    char b[160];
    va_list a;

    va_start(a,fmt);
    int len=vsnprintf(b,sizeof(b),fmt,a);
    va_end(a);
    if(len>0)Det_Print(b);
}
