#include "PduR.h"
#include "CanIf.h"
#include "ComIf.h"
#include "Det.h"

static const PduR_ConfigType *PduR_ConfigPtr = NULL_PTR;
static boolean PduR_Initialized = FALSE;

void PduR_Init(const PduR_ConfigType *ConfigPtr)
{
    PduR_ConfigPtr = (ConfigPtr != NULL_PTR) ? ConfigPtr : &PduR_Config;
    PduR_Initialized = TRUE;
}

Std_ReturnType PduR_ComTransmit(PduIdType TxPduId, const PduInfoType *PduInfoPtr)
{
    CanIf_PduType canPdu;
    const PduR_TxRouteType *route;

    if (PduR_Initialized == FALSE) {
        (void)Det_ReportError(PDUR_MODULE_ID, PDUR_INSTANCE_ID, PDUR_SID_COM_TRANSMIT, PDUR_E_UNINIT);
        return E_NOT_OK;
    }

    if ((PduInfoPtr == NULL_PTR) || (PduInfoPtr->SduDataPtr == NULL_PTR)) {
        (void)Det_ReportError(PDUR_MODULE_ID, PDUR_INSTANCE_ID, PDUR_SID_COM_TRANSMIT, PDUR_E_PARAM_POINTER);
        return E_NOT_OK;
    }

    if ((PduR_ConfigPtr == NULL_PTR) || (TxPduId >= PduR_ConfigPtr->txRouteCount)) {
        (void)Det_ReportError(PDUR_MODULE_ID, PDUR_INSTANCE_ID, PDUR_SID_COM_TRANSMIT, PDUR_E_PDU_ID_INVALID);
        return E_NOT_OK;
    }

    route = &PduR_ConfigPtr->txRoutes[TxPduId];

    canPdu.id = route->canId;
    canPdu.length = (uint8)PduInfoPtr->SduLength;
    canPdu.sdu = PduInfoPtr->SduDataPtr;
    canPdu.swPduHandle = route->destPduId;

    return CanIf_Transmit(route->canHth, &canPdu);
}

void PduR_CanIfRxIndication(uint16 CanId, const uint8 *SduDataPtr, uint8 SduLength)
{
    if ((PduR_Initialized == FALSE) || (PduR_ConfigPtr == NULL_PTR) || (SduDataPtr == NULL_PTR)) {
        return;
    }

    for (uint16 i = 0U; i < PduR_ConfigPtr->rxRouteCount; i++) {
        const PduR_RxRouteType *route = &PduR_ConfigPtr->rxRoutes[i];
        if (route->canId == CanId) {
            ComIf_RxIndication(route->destPduId, SduDataPtr, SduLength);
            return;
        }
    }
}

void PduR_MainFunction(void)
{
    /* Direct IF routing only; no TP gateway/buffer for this educational stack. */
}
