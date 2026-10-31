#include "BareCan.h"
#include "Can.h"
#include "Can_Cfg.h"
#include "Project_Cfg.h"

static BareCan_MainCommandType BareCan_LastMainCommand;
static boolean BareCan_HasNewMainCommand = FALSE;

void BareCan_Init(void)
{
    BareCan_HasNewMainCommand = FALSE;

    BareCan_LastMainCommand.adcRaw = 0U;
    BareCan_LastMainCommand.brake = 0U;
    BareCan_LastMainCommand.direction = 0U;
    BareCan_LastMainCommand.sequence = 0U;
    BareCan_LastMainCommand.dlc = 0U;
}

void BareCan_RxIndication(
    uint16 canId,
    const uint8 *data,
    uint8 dlc
)
{
    if (data == NULL_PTR) {
        return;
    }

    if (canId != CAN_ID_MAIN_TO_ABS) {
        return;
    }

    if (dlc < 4U) {
        return;
    }

    BareCan_LastMainCommand.adcRaw =
        (uint16)((uint16)data[0] |
        (((uint16)data[1] & 0x0FU) << 8U));

    BareCan_LastMainCommand.brake = data[2];
    BareCan_LastMainCommand.direction = data[3];

    BareCan_LastMainCommand.sequence =
        (dlc >= 5U) ? data[4] : 0U;

    BareCan_LastMainCommand.dlc = dlc;

    BareCan_HasNewMainCommand = TRUE;
}

Std_ReturnType BareCan_ReadMainCommand(
    BareCan_MainCommandType *cmd
)
{
    if (cmd == NULL_PTR) {
        return E_NOT_OK;
    }

    if (BareCan_HasNewMainCommand == FALSE) {
        return E_NOT_OK;
    }

    *cmd = BareCan_LastMainCommand;
    BareCan_HasNewMainCommand = FALSE;

    return E_OK;
}

static Std_ReturnType BareCan_SendFrame(
    uint16 id,
    const uint8 *data,
    uint8 dlc
)
{
    Can_PduType pdu;

    if ((data == NULL_PTR) || (dlc > 8U)) {
        return E_NOT_OK;
    }

    pdu.id = id;
    pdu.length = dlc;
    pdu.sdu = data;
    pdu.swPduHandle = 0U;

    if (Can_Write(
        CAN_MAIN_HOH_ABS_TX,
        &pdu
    ) == CAN_OK) {
        return E_OK;
    }

    return E_NOT_OK;
}

Std_ReturnType BareCan_SendAbsFeedback(
    uint8 wheel1Speed,
    uint8 wheel2Speed,
    uint8 motorState,
    uint8 sequence
)
{
    uint8 data[4];

    data[0] = wheel1Speed;
    data[1] = wheel2Speed;
    data[2] = motorState;
    data[3] = sequence;

    return BareCan_SendFrame(
        CAN_ID_ABS_TO_MAIN,
        data,
        4U
    );
}

Std_ReturnType BareCan_SendCanDiagStatus(
    const Can_DebugStatusType *status
)
{
    uint8 data[8];

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

    return BareCan_SendFrame(
        CAN_ID_CAN_DIAG_STATUS,
        data,
        8U
    );
}
