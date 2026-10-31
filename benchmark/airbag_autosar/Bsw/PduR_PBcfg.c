#include "PduR.h"
#include "CanIf_Cfg.h"
#include "ComIf_Cfg.h"

static const PduR_TxRouteType PduR_TxRoutes[PDUR_TX_ROUTE_COUNT] = {
    { CAN_ID_AIRBAG_TO_MAIN,  CANIF_HTH_AIRBAG_TX, PDUR_PDU_AIRBAG_DEPLOY_FLAG },
    { CAN_ID_CAN_DIAG_STATUS, CANIF_HTH_AIRBAG_TX, PDUR_PDU_CAN_DIAG_STATUS }
};

const PduR_ConfigType PduR_Config = {
    PduR_TxRoutes,
    PDUR_TX_ROUTE_COUNT
};
