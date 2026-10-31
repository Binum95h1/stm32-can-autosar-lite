#include "Rte_Main.h"
#include "Adc.h"
#include "Dio.h"
#include "CanIf.h"
#include "Can_Cfg.h"
#include "OsIf.h"
#include "Det.h"
#include "LcdIf.h"
#include <stdarg.h>
#include <stdio.h>

uint16 Rte_Call_MainS_Adc_ReadThrottleRaw(void)
{
    return (uint16)Adc_Read();
}

uint8 Rte_Call_MainS_Dio_ReadBrakeSwitch(void)
{
    return Dio_ReadChannel(DIO_CHANNEL_BRAKE_SW);
}

uint8 Rte_Call_MainS_Dio_ReadDirectionSwitch(void)
{
    return Dio_ReadChannel(DIO_CHANNEL_DIRECTION_SW);
}

Std_ReturnType Rte_Call_MainS_Com_SendMainCommand(uint16 adcRaw, uint8 brake, uint8 direction, uint8 sequence)
{
    return ComIf_SendMainCommand(adcRaw, brake, direction, sequence);
}

Std_ReturnType Rte_Call_MainS_Com_SendCanDiagStatus(const Can_DebugStatusType *status)
{
    return ComIf_SendCanDiagStatus(status);
}

Std_ReturnType Rte_Read_MainS_AbsFeedback(ComIf_AbsFeedbackType *feedback)
{
    return ComIf_ReadAbsFeedback(feedback);
}

Std_ReturnType Rte_Read_MainS_AirbagStatus(ComIf_AirbagStatusType *status)
{
    return ComIf_ReadAirbagStatus(status);
}

Std_ReturnType Rte_Call_MainS_CanIf_GetDebugStatus(Can_DebugStatusType *status)
{
    return CanIf_GetDebugStatus(CAN_MAIN_CONTROLLER, status);
}

uint32 Rte_Call_MainS_OsIf_GetTimeMs(void)
{
    return OsIf_GetTimeMs();
}

void Rte_Call_MainS_OsIf_DelayMs(uint32 ms)
{
    OsIf_DelayMs(ms);
}

void Rte_Call_MainS_Det_Log(const char *s)
{
    Det_Print(s);
}

void Rte_Call_MainS_Det_Logf(const char *fmt, ...)
{
    char b[220];
    va_list a;
    int len;

    va_start(a, fmt);
    len = vsnprintf(b, sizeof(b), fmt, a);
    va_end(a);

    if (len > 0) {
        Det_Print(b);
    }
}

void Rte_Call_MainS_Lcd_Init(void)
{
    LcdIf_Init();
}

void Rte_Call_MainS_Lcd_Clear(void)
{
    LcdIf_Clear();
}

void Rte_Call_MainS_Lcd_SetCursor(uint8 row, uint8 col)
{
    LcdIf_SetCursor(row, col);
}

void Rte_Call_MainS_Lcd_SendString(const char *s)
{
    LcdIf_SendString(s);
}
