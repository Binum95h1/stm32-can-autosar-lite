#ifndef CANIF_TYPES_H
#define CANIF_TYPES_H

#include "Std_Types.h"
#include "Can_Types.h"

typedef uint8 CanIf_HthType;
typedef uint16 CanIf_IdType;

typedef struct {
    CanIf_IdType id;
    uint8 length;
    const uint8 *sdu;
    uint16 swPduHandle;
} CanIf_PduType;

typedef struct {
    CanIf_HthType hth;
    Can_HwHandleType canHoh;
} CanIf_TxHthConfigType;

typedef struct {
    const CanIf_TxHthConfigType *txHthConfig;
    uint8 txHthCount;
} CanIf_ConfigType;

#endif /* CANIF_TYPES_H */
