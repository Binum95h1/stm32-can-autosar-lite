#include "Can_UpperLayer.h"
#include "BareCan.h"

void Can_UpperRxIndication(
    uint8 controller,
    uint16 canId,
    const uint8 *data,
    uint8 dlc
)
{
    (void)controller;

    BareCan_RxIndication(
        canId,
        data,
        dlc
    );
}
