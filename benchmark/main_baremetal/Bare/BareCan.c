#include "BareCan.h"
#include "Can.h"
#include "Can_Cfg.h"
#include "Project_Cfg.h"
static BareCan_AbsFeedbackType last_abs;
static BareCan_AirbagStatusType last_airbag;
static boolean abs_pending = FALSE;
static boolean airbag_pending = FALSE;
void BareCan_Init(void)
{
    BareCan_AbsFeedbackType a = {0};
    BareCan_AirbagStatusType b = {0};
    last_abs = a;
    last_airbag = b;
    abs_pending = FALSE;
    airbag_pending = FALSE;
}
static Std_ReturnType BareCan_Send(uint16 id, const uint8 *data, uint8 dlc)
{
    Can_PduType pdu;
    pdu.id = id;
    pdu.length = dlc;
    pdu.sdu = data;
    pdu.swPduHandle = 0U;
    return (Can_Write(CAN_MAIN_HOH_MAIN_TX, &pdu) == CAN_OK) ? E_OK : E_NOT_OK;
}
Std_ReturnType BareCan_SendMainCommand(uint16 adcRaw, uint8 brake, uint8 direction, uint8 sequence)
{
    uint8 data[5];
    data[0] = (uint8)(adcRaw & 0xFFU);
    data[1] = (uint8)((adcRaw >> 8U) & 0x0FU);
    data[2] = brake;
    data[3] = direction;
    data[4] = sequence;
    return BareCan_Send(CAN_ID_MAIN_TO_ABS, data, 5U);
}
Std_ReturnType BareCan_SendCanDiagStatus(const Can_DebugStatusType *s)
{
    uint8 data[8];
    if (s == NULL_PTR) return E_NOT_OK;
    data[0] = s->lec; data[1] = s->tec;
    data[2] = s->rec; data[3] = s->ewgf;
    data[4] = s->epvf; data[5] = s->boff;
    data[6] = (uint8)s->errorState;
    data[7] = (uint8)s->controllerMode;
    return BareCan_Send(CAN_ID_CAN_DIAG_STATUS, data, 8U);
}
Std_ReturnType BareCan_ReadAbsFeedback(BareCan_AbsFeedbackType *f)
{
    if ((f == NULL_PTR) || (abs_pending == FALSE)) return E_NOT_OK;
    *f = last_abs;
    abs_pending = FALSE;
    return E_OK;
}
Std_ReturnType BareCan_ReadAirbagStatus(BareCan_AirbagStatusType *s)
{
    if ((s == NULL_PTR) || (airbag_pending == FALSE)) return E_NOT_OK;
    *s = last_airbag;
    airbag_pending = FALSE;
    return E_OK;
}
void BareCan_RxIndication(uint16 id, const uint8 *d, uint8 dlc)
{
    if ((d == NULL_PTR) || (dlc > 8U)) return;
    if ((id == CAN_ID_ABS_TO_MAIN) && (dlc >= 3U)) {
        last_abs.wheel1Speed = d[0];
        last_abs.wheel2Speed = d[1];
        last_abs.motorState = d[2];
        last_abs.sequence = (dlc >= 4U) ? d[3] : 0U;
        last_abs.dlc = dlc;
        abs_pending = TRUE;
    } else if ((id == CAN_ID_AIRBAG_TO_MAIN) && (dlc >= 1U)) {
        last_airbag.deploy = d[0];
        last_airbag.sequence = (dlc >= 2U) ? d[1] : 0U;
        last_airbag.reserved = (dlc >= 3U) ? d[2] : 0U;
        last_airbag.dlc = dlc;
        airbag_pending = TRUE;
    }
}
