#include "PduR.h"
#include "CanIf_Cfg.h"
#include "ComIf_Cfg.h"

static const PduR_TxRouteType PduR_TxRoutes[PDUR_TX_ROUTE_COUNT] = {
    { CAN_ID_ABS_TO_MAIN,      CANIF_HTH_ABS_TX, PDUR_PDU_ABS_TO_MAIN },
    { CAN_ID_CAN_DIAG_STATUS,  CANIF_HTH_ABS_TX, PDUR_PDU_CAN_DIAG_STATUS }
};

static const PduR_RxRouteType PduR_RxRoutes[PDUR_RX_ROUTE_COUNT] = {
    { CAN_ID_MAIN_TO_ABS, PDUR_PDU_MAIN_TO_ABS }
};

const PduR_ConfigType PduR_Config = {
    PduR_TxRoutes,
    PDUR_TX_ROUTE_COUNT,
    PduR_RxRoutes,
    PDUR_RX_ROUTE_COUNT
};
