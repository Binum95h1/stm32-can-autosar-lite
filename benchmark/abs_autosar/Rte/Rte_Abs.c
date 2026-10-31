#include "Rte_Abs.h"
#include "ComIf.h"
#include "CanIf.h"
#include "Can_Cfg.h"
#include "OsIf.h"
#include "Det.h"
#include "MotorIf.h"
#include <stdarg.h>
#include <stdio.h>

Std_ReturnType Rte_Read_AbsS_MainCommand(ComIf_MainCommandType *cmd){
    return ComIf_ReadMainCommand(cmd);
}

Std_ReturnType Rte_Call_AbsS_Com_SendAbsFeedback(uint8 wheel1Speed, uint8 wheel2Speed, uint8 motorState, uint8 sequence){
    return ComIf_SendAbsFeedback(wheel1Speed, wheel2Speed, motorState, sequence);
}

Std_ReturnType Rte_Call_AbsS_Com_SendCanDiagStatus(const Can_DebugStatusType *status){
    return ComIf_SendCanDiagStatus(status);
}

Std_ReturnType Rte_Call_AbsS_CanIf_GetDebugStatus(Can_DebugStatusType *status){
    return CanIf_GetDebugStatus(CAN_MAIN_CONTROLLER, status);
}

uint32 Rte_Call_AbsS_OsIf_GetTimeMs(void){
    return OsIf_GetTimeMs();
}

void Rte_Call_AbsS_OsIf_DelayMs(uint32 ms){
    OsIf_DelayMs(ms);
}

void Rte_Call_AbsS_Det_Log(const char *s){
    Det_Print(s);
}

void Rte_Call_AbsS_Det_Logf(const char *fmt, ...)
{
    char b[180];
    va_list a;
    int len;

    va_start(a, fmt);
    len = vsnprintf(b, sizeof(b), fmt, a);
    va_end(a);

    if (len > 0) {
        Det_Print(b);
    }
}

void Rte_Call_AbsS_Motor_SetPwm(uint16 ccr1, uint16 ccr2){
    MotorIf_SetPwm(ccr1, ccr2);
}

uint16 Rte_Call_AbsS_Motor_GetPwmPeriod(void){
    return MotorIf_GetPwmPeriod();
}

uint16 Rte_Call_AbsS_Motor_GetEncoderCount1(void){
    return MotorIf_GetEncoderCount1();
}

uint16 Rte_Call_AbsS_Motor_GetEncoderCount2(void){
    return MotorIf_GetEncoderCount2();
}

void Rte_Call_AbsS_Motor_SetDirection1(uint8 in1, uint8 in2){
    MotorIf_SetDirection1(in1, in2);
}

void Rte_Call_AbsS_Motor_SetDirection2(uint8 in1, uint8 in2){
    MotorIf_SetDirection2(in1, in2);
}
