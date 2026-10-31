#include "ComIf.h"
#include "PduR.h"

static ComIf_MainCommandType ComIf_LastMainCommand;
static boolean ComIf_HasNewMainCommand = FALSE;

void ComIf_Init(void)
{
    ComIf_HasNewMainCommand = FALSE;
    ComIf_LastMainCommand.adcRaw = 0U;
    ComIf_LastMainCommand.brake = 0U;
    ComIf_LastMainCommand.direction = 0U;
    ComIf_LastMainCommand.sequence = 0U;
    ComIf_LastMainCommand.dlc = 0U;
}

Std_ReturnType ComIf_SendAbsFeedback(uint8 wheel1Speed, uint8 wheel2Speed, uint8 motorState, uint8 sequence)
{
    uint8 data[4];
    PduInfoType pdu = { data, 4U };

    data[0] = wheel1Speed;
    data[1] = wheel2Speed;
    data[2] = motorState;
    data[3] = sequence;

    return PduR_ComTransmit(PDUR_PDU_ABS_TO_MAIN, &pdu);
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

Std_ReturnType ComIf_ReadMainCommand(ComIf_MainCommandType *cmd)
{
    if (cmd == NULL_PTR) {
        return E_NOT_OK;
    }

    if (ComIf_HasNewMainCommand == FALSE) {
        return E_NOT_OK;
    }

    *cmd = ComIf_LastMainCommand;
    ComIf_HasNewMainCommand = FALSE;
    return E_OK;
}

void ComIf_RxIndication(PduIdType RxPduId, const uint8 *SduDataPtr, uint8 SduLength)
{
    if ((SduDataPtr == NULL_PTR) || (RxPduId != PDUR_PDU_MAIN_TO_ABS) || (SduLength < 4U)) {
        return;
    }

    ComIf_LastMainCommand.adcRaw = (uint16)((uint16)SduDataPtr[0] | (((uint16)SduDataPtr[1] & 0x0FU) << 8U));
    ComIf_LastMainCommand.brake = SduDataPtr[2];
    ComIf_LastMainCommand.direction = SduDataPtr[3];
    ComIf_LastMainCommand.sequence = (SduLength >= 5U) ? SduDataPtr[4] : 0U;
    ComIf_LastMainCommand.dlc = SduLength;
    ComIf_HasNewMainCommand = TRUE;
}
