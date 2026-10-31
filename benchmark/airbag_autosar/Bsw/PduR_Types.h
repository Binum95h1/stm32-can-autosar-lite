#ifndef PDUR_TYPES_H
#define PDUR_TYPES_H

#include "Std_Types.h"
#include "CanIf_Types.h"

typedef uint16 PduIdType;
typedef uint16 PduLengthType;

typedef struct {
    uint8 *SduDataPtr;
    PduLengthType SduLength;
} PduInfoType;

typedef struct {
    uint16 canId;
    CanIf_HthType canHth;
    PduIdType destPduId;
} PduR_TxRouteType;

typedef struct {
    const PduR_TxRouteType *txRoutes;
    uint16 txRouteCount;
} PduR_ConfigType;

#endif
