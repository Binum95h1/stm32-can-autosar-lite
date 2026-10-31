#include "CanIf.h"
#include "Can.h"
#include "PduR.h"
#include "SchM.h"
#include "Det.h"

#define CANIF_MODULE_ID           ((uint16)60U)
#define CANIF_INSTANCE_ID         ((uint8)0U)
#define CANIF_SID_TRANSMIT        ((uint8)0x49U)
#define CANIF_E_UNINIT            ((uint8)0x30U)
#define CANIF_E_PARAM_POINTER     ((uint8)0x20U)
#define CANIF_E_INVALID_TXPDUID   ((uint8)0x50U)

static const CanIf_ConfigType *CanIf_ConfigPtr = NULL_PTR;
static boolean CanIf_Initialized = FALSE;

#define CANIF_START_SEC_CODE
#include "MemMap.h"

void CanIf_Init(const CanIf_ConfigType *ConfigPtr){
    CanIf_ConfigPtr = (ConfigPtr != NULL_PTR) ? ConfigPtr : &CanIf_Config;
    CanIf_Initialized = TRUE;
}

Std_ReturnType CanIf_Transmit(CanIf_HthType Hth, const CanIf_PduType *PduInfoPtr){
    Can_PduType canPdu;
    Can_ReturnType canRet;

    if (CanIf_Initialized == FALSE){
        (void)Det_ReportError(CANIF_MODULE_ID, CANIF_INSTANCE_ID, CANIF_SID_TRANSMIT, CANIF_E_UNINIT);
        return E_NOT_OK;
    }

    if ((PduInfoPtr == NULL_PTR) || (PduInfoPtr->sdu == NULL_PTR)) {
        (void)Det_ReportError(CANIF_MODULE_ID, CANIF_INSTANCE_ID, CANIF_SID_TRANSMIT, CANIF_E_PARAM_POINTER);
        return E_NOT_OK;
    }

    if ((CanIf_ConfigPtr == NULL_PTR) || (PduInfoPtr->length > 8U) || (Hth >= CanIf_ConfigPtr->txHthCount)) {
        (void)Det_ReportError(CANIF_MODULE_ID, CANIF_INSTANCE_ID, CANIF_SID_TRANSMIT, CANIF_E_INVALID_TXPDUID);
        return E_NOT_OK;
    }

    canPdu.id = PduInfoPtr->id;
    canPdu.length = PduInfoPtr->length;
    canPdu.sdu = PduInfoPtr->sdu;
    canPdu.swPduHandle = PduInfoPtr->swPduHandle;

    SchM_Enter_CanIf_EXCLUSIVE_AREA_0();
    canRet = Can_Write(CanIf_ConfigPtr->txHthConfig[Hth].canHoh, &canPdu);
    SchM_Exit_CanIf_EXCLUSIVE_AREA_0();

    return (canRet == CAN_OK) ? E_OK : E_NOT_OK;
}

Std_ReturnType CanIf_SetControllerMode(uint8 ControllerId, Can_ControllerStateType ControllerMode){
    return Can_SetControllerMode(ControllerId, ControllerMode);
}

Std_ReturnType CanIf_GetControllerMode(uint8 ControllerId, Can_ControllerStateType *ControllerModePtr){
    return Can_GetControllerMode(ControllerId, ControllerModePtr);
}

Std_ReturnType CanIf_GetControllerErrorState(uint8 ControllerId, Can_ErrorStateType *ErrorStatePtr)
{
    return Can_GetControllerErrorState(ControllerId, ErrorStatePtr);
}

Std_ReturnType CanIf_GetDebugStatus(uint8 ControllerId, Can_DebugStatusType *StatusPtr)
{
    return Can_GetDebugStatus(ControllerId, StatusPtr);
}

void CanIf_RxIndication(uint8 ControllerId, uint16 CanId, const uint8 *SduDataPtr, uint8 SduLength){
    (void)ControllerId;
    PduR_CanIfRxIndication(CanId, SduDataPtr, SduLength);
}

void CanIf_ControllerBusOff(uint8 ControllerId){
    (void)ControllerId;
    Det_Print("CanIf: controller bus-off indication\r\n");
}

void CanIf_MainFunction(void)
{
    Can_MainFunction_BusOff();
    Can_MainFunction_Mode();
    Can_MainFunction_Write();
    Can_MainFunction_Read();
}

#define CANIF_STOP_SEC_CODE
#include "MemMap.h"
