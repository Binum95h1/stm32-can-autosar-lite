#ifndef RTE_MAIN_H
#define RTE_MAIN_H

#include "Std_Types.h"
#include "ComIf.h"
#include "Can_Types.h"

uint16 Rte_Call_MainS_Adc_ReadThrottleRaw(void);
uint8 Rte_Call_MainS_Dio_ReadBrakeSwitch(void);
uint8 Rte_Call_MainS_Dio_ReadDirectionSwitch(void);
Std_ReturnType Rte_Call_MainS_Com_SendMainCommand(uint16 adcRaw, uint8 brake, uint8 direction, uint8 sequence);
Std_ReturnType Rte_Call_MainS_Com_SendCanDiagStatus(const Can_DebugStatusType *status);
Std_ReturnType Rte_Read_MainS_AbsFeedback(ComIf_AbsFeedbackType *feedback);
Std_ReturnType Rte_Read_MainS_AirbagStatus(ComIf_AirbagStatusType *status);
Std_ReturnType Rte_Call_MainS_CanIf_GetDebugStatus(Can_DebugStatusType *status);
uint32 Rte_Call_MainS_OsIf_GetTimeMs(void);
void Rte_Call_MainS_OsIf_DelayMs(uint32 ms);
void Rte_Call_MainS_Det_Log(const char *s);
void Rte_Call_MainS_Det_Logf(const char *fmt, ...);
void Rte_Call_MainS_Lcd_Init(void);
void Rte_Call_MainS_Lcd_Clear(void);
void Rte_Call_MainS_Lcd_SetCursor(uint8 row, uint8 col);
void Rte_Call_MainS_Lcd_SendString(const char *s);

#endif /* RTE_MAIN_H */
