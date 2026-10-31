#ifndef RTE_ABS_H
#define RTE_ABS_H

#include "Std_Types.h"
#include "ComIf.h"
#include "Can_Types.h"

Std_ReturnType Rte_Read_AbsS_MainCommand(ComIf_MainCommandType *cmd);
Std_ReturnType Rte_Call_AbsS_Com_SendAbsFeedback(uint8 wheel1Speed, uint8 wheel2Speed, uint8 motorState, uint8 sequence);
Std_ReturnType Rte_Call_AbsS_Com_SendCanDiagStatus(const Can_DebugStatusType *status);
Std_ReturnType Rte_Call_AbsS_CanIf_GetDebugStatus(Can_DebugStatusType *status);
uint32 Rte_Call_AbsS_OsIf_GetTimeMs(void);
void Rte_Call_AbsS_OsIf_DelayMs(uint32 ms);
void Rte_Call_AbsS_Det_Log(const char *s);
void Rte_Call_AbsS_Det_Logf(const char *fmt, ...);
void Rte_Call_AbsS_Motor_SetPwm(uint16 ccr1, uint16 ccr2);
uint16 Rte_Call_AbsS_Motor_GetPwmPeriod(void);
uint16 Rte_Call_AbsS_Motor_GetEncoderCount1(void);
uint16 Rte_Call_AbsS_Motor_GetEncoderCount2(void);
void Rte_Call_AbsS_Motor_SetDirection1(uint8 in1, uint8 in2);
void Rte_Call_AbsS_Motor_SetDirection2(uint8 in1, uint8 in2);

#endif /* RTE_ABS_H */
