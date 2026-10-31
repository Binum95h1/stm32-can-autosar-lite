#ifndef CANIF_H
#define CANIF_H

#include "Std_Types.h"
#include "CanIf_Types.h"
#include "CanIf_Cfg.h"

extern const CanIf_ConfigType CanIf_Config;

void CanIf_Init(const CanIf_ConfigType *ConfigPtr);
Std_ReturnType CanIf_Transmit(CanIf_HthType Hth, const CanIf_PduType *PduInfoPtr);
Std_ReturnType CanIf_SetControllerMode(uint8 ControllerId, Can_ControllerStateType ControllerMode);
Std_ReturnType CanIf_GetControllerMode(uint8 ControllerId, Can_ControllerStateType *ControllerModePtr);
Std_ReturnType CanIf_GetControllerErrorState(uint8 ControllerId, Can_ErrorStateType *ErrorStatePtr);
Std_ReturnType CanIf_GetDebugStatus(uint8 ControllerId, Can_DebugStatusType *StatusPtr);
void CanIf_MainFunction(void);
void CanIf_ControllerBusOff(uint8 ControllerId);

#endif
