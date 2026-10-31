#ifndef CAN_H
#define CAN_H
#include "Std_Types.h"
#include "Can_Types.h"
#include "Can_Cfg.h"

void Can_Init(const Can_ConfigType *ConfigPtr);
void Can_DeInit(void);
Std_ReturnType Can_SetControllerMode(uint8 Controller, Can_ControllerStateType Transition);
Std_ReturnType Can_GetControllerMode(uint8 Controller, Can_ControllerStateType *ControllerModePtr);
Std_ReturnType Can_GetControllerErrorState(uint8 Controller, Can_ErrorStateType *ErrorStatePtr);
Std_ReturnType Can_GetDebugStatus(uint8 Controller, Can_DebugStatusType *StatusPtr);
Can_ReturnType Can_Write(Can_HwHandleType Hth, const Can_PduType *PduInfo);
void Can_MainFunction_BusOff(void);
void Can_MainFunction_Mode(void);
void Can_MainFunction_Write(void);
void Can_MainFunction_Read(void);
void Can_Init_500kbps(void);
uint8 Can_WriteStd(uint16 id, const uint8 *data, uint8 dlc);

#endif
