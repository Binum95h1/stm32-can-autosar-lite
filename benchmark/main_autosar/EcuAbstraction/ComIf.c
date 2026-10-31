#include "ComIf.h"
#include "PduR.h"

static ComIf_AbsFeedbackType ComIf_LastAbsFeedback;
static ComIf_AirbagStatusType ComIf_LastAirbagStatus;
static boolean ComIf_HasNewAbsFeedback = FALSE;
static boolean ComIf_HasNewAirbagStatus = FALSE;

void ComIf_Init(void)
{
    ComIf_HasNewAbsFeedback = FALSE;
    ComIf_HasNewAirbagStatus = FALSE;

    ComIf_LastAbsFeedback.wheel1Speed = 0U;
    ComIf_LastAbsFeedback.wheel2Speed = 0U;
    ComIf_LastAbsFeedback.motorState = 0U;
    ComIf_LastAbsFeedback.sequence = 0U;
    ComIf_LastAbsFeedback.dlc = 0U;

    ComIf_LastAirbagStatus.deploy = 0U;
    ComIf_LastAirbagStatus.sequence = 0U;
    ComIf_LastAirbagStatus.reserved = 0U;
    ComIf_LastAirbagStatus.dlc = 0U;
}

Std_ReturnType ComIf_SendMainCommand(uint16 adcRaw, uint8 brake, uint8 direction, uint8 sequence)
{
    uint8 data[5];
    PduInfoType pdu = { data, 5U };

    data[0] = (uint8)(adcRaw & 0xFFU);
    data[1] = (uint8)((adcRaw >> 8U) & 0x0FU);
    data[2] = brake;
    data[3] = direction;
    data[4] = sequence;

    return PduR_ComTransmit(PDUR_PDU_MAIN_TO_ABS, &pdu);
}

Std_ReturnType ComIf_SendCanDiagStatus(const Can_DebugStatusType *status)
{
    uint8 data[8];
    PduInfoType pdu = { data, 8U };

    if (status == NULL_PTR) {
        return E_NOT_OK;
    }

    data[0] = status->lec;
    data[1] = status->tec;
    data[2] = status->rec;
    data[3] = status->ewgf;
    data[4] = status->epvf;
    data[5] = status->boff;
    data[6] = (uint8)status->errorState;
    data[7] = (uint8)status->controllerMode;

    return PduR_ComTransmit(PDUR_PDU_CAN_DIAG_STATUS, &pdu);
}

Std_ReturnType ComIf_ReadAbsFeedback(ComIf_AbsFeedbackType *feedback)
{
    if (feedback == NULL_PTR) {
        return E_NOT_OK;
    }

    if (ComIf_HasNewAbsFeedback == FALSE) {
        return E_NOT_OK;
    }

    *feedback = ComIf_LastAbsFeedback;
    ComIf_HasNewAbsFeedback = FALSE;
    return E_OK;
}

Std_ReturnType ComIf_ReadAirbagStatus(ComIf_AirbagStatusType *status)
{
    if (status == NULL_PTR) {
        return E_NOT_OK;
    }

    if (ComIf_HasNewAirbagStatus == FALSE) {
        return E_NOT_OK;
    }

    *status = ComIf_LastAirbagStatus;
    ComIf_HasNewAirbagStatus = FALSE;
    return E_OK;
}

void ComIf_RxIndication(PduIdType RxPduId, const uint8 *SduDataPtr, uint8 SduLength)
{
    if (SduDataPtr == NULL_PTR) {
        return;
    }

    switch (RxPduId) {
    case PDUR_PDU_ABS_TO_MAIN:
        if (SduLength >= 3U) {
            ComIf_LastAbsFeedback.wheel1Speed = SduDataPtr[0];
            ComIf_LastAbsFeedback.wheel2Speed = SduDataPtr[1];
            ComIf_LastAbsFeedback.motorState = SduDataPtr[2];
            ComIf_LastAbsFeedback.sequence = (SduLength >= 4U) ? SduDataPtr[3] : 0U;
            ComIf_LastAbsFeedback.dlc = SduLength;
            ComIf_HasNewAbsFeedback = TRUE;
        }
        break;

    case PDUR_PDU_AIRBAG_TO_MAIN:
        if (SduLength >= 1U) {
            ComIf_LastAirbagStatus.deploy = SduDataPtr[0];
            ComIf_LastAirbagStatus.sequence = (SduLength >= 2U) ? SduDataPtr[1] : 0U;
            ComIf_LastAirbagStatus.reserved = (SduLength >= 3U) ? SduDataPtr[2] : 0U;
            ComIf_LastAirbagStatus.dlc = SduLength;
            ComIf_HasNewAirbagStatus = TRUE;
        }
        break;

    default:
        break;
    }
}
