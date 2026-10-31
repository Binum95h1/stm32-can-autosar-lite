#include "CanIf.h"
#include "Can_Cfg.h"

static const CanIf_TxHthConfigType CanIf_TxHthConfig[CANIF_TX_HTH_COUNT] = {
    { 
        CANIF_HTH_MAIN_TX, CAN_MAIN_HOH_MAIN_TX 
    }
};

const CanIf_ConfigType CanIf_Config = {
    CanIf_TxHthConfig,
    CANIF_TX_HTH_COUNT
};
