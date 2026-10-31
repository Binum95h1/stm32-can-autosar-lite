#include "ComIf.h"
#include "PduR.h"

static uint8 ComIf_DeploySeqCounter = 0U;
static uint32 ComIf_DeployTxOkCount = 0U;
static uint32 ComIf_DeployTxFailCount = 0U;

void ComIf_Init(void)
{
    /* COM abstraction init hook. Lower layers are initialized by EcuM. */
}

Std_ReturnType ComIf_SendDeployFlag(boolean deploy_flag)
{
    uint8 data[2];
    PduInfoType pdu = { data, 2U };
    Std_ReturnType ret;

    /*
     * Byte0 is kept as deploy flag for backward compatibility with the old
     * receiver. Byte1 is a sequence counter used for Chapter 4 frame-loss tests.
     */
    data[0] = (deploy_flag != FALSE) ? 1U : 0U;
    data[1] = ComIf_DeploySeqCounter;

    ret = PduR_ComTransmit(PDUR_PDU_AIRBAG_DEPLOY_FLAG, &pdu);
    if (ret == E_OK) {
        ComIf_DeployTxOkCount++;
        ComIf_DeploySeqCounter++;
    } else {
        ComIf_DeployTxFailCount++;
    }

    return ret;
}

uint32 ComIf_GetDeployTxOkCount(void)
{
    return ComIf_DeployTxOkCount;
}

uint32 ComIf_GetDeployTxFailCount(void)
{
    return ComIf_DeployTxFailCount;
}

uint8 ComIf_GetDeploySeqCounter(void)
{
    return ComIf_DeploySeqCounter;
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
