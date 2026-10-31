#include "BareCan.h"
#include "Can.h"
#include "Can_Cfg.h"
static uint8 tx_sequence;
static uint32 tx_ok;
static uint32 tx_fail;
void BareCan_Init(void) { tx_sequence=0U; tx_ok=0U; tx_fail=0U; }
Std_ReturnType BareCan_SendDeployFlag(boolean deploy_flag) {
    uint8 data[2] = { (deploy_flag!=FALSE)?1U:0U, tx_sequence };
    Can_PduType pdu = { CAN_ID_AIRBAG_TO_MAIN, 2U, data, 0U };
    if (Can_Write(CAN_MAIN_HOH_AIRBAG_TX,&pdu)==CAN_OK) { tx_ok++; tx_sequence++; return E_OK; }
    tx_fail++;
    return E_NOT_OK;
}
Std_ReturnType BareCan_SendCanDiagStatus(const Can_DebugStatusType *status) {
    uint8 data[8];
    Can_PduType pdu;
    if (status==NULL_PTR) return E_NOT_OK;
    data[0]=status->lec; data[1]=status->tec; data[2]=status->rec;
    data[3]=status->ewgf; data[4]=status->epvf; data[5]=status->boff;
    data[6]=(uint8)status->errorState; data[7]=(uint8)status->controllerMode;
    pdu.id=CAN_ID_CAN_DIAG_STATUS; pdu.length=8U; pdu.sdu=data; pdu.swPduHandle=0U;
    return Can_Write(CAN_MAIN_HOH_AIRBAG_TX,&pdu)==CAN_OK ? E_OK : E_NOT_OK;
}
uint32 BareCan_GetDeployTxOkCount(void){return tx_ok;}
uint32 BareCan_GetDeployTxFailCount(void){return tx_fail;}
uint8 BareCan_GetDeploySeqCounter(void){return tx_sequence;}
