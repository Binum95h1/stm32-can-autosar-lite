#ifndef CAN_UPPER_LAYER_H
#define CAN_UPPER_LAYER_H

#include "Std_Types.h"

void Can_UpperRxIndication(
    uint8 controller,
    uint16 canId,
    const uint8 *data,
    uint8 dlc
);

#endif
