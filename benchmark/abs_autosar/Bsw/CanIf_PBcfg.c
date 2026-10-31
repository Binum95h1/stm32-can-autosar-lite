#include "CanIf.h"
#include "Can_Cfg.h"

static const CanIf_TxHthConfigType CanIf_TxHthConfig[CANIF_TX_HTH_COUNT] = {
    { 
        CANIF_HTH_ABS_TX, CAN_MAIN_HOH_ABS_TX 
    }
};

const CanIf_ConfigType CanIf_Config = {
    CanIf_TxHthConfig,
    CANIF_TX_HTH_COUNT
};
