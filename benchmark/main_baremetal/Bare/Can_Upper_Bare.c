#include "Can_UpperLayer.h"
#include "BareCan.h"
void Can_UpperRxIndication(uint8 controller, uint16 id, const uint8 *data, uint8 dlc)
{
    (void)controller;
    BareCan_RxIndication(id, data, dlc);
}
void Can_UpperControllerBusOff(uint8 controller)
{
    (void)controller;
    /* The common register-level driver already logs Bus-Off. */
}
