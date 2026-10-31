#include "Can_UpperLayer.h"
#include "CanIf.h"
void Can_UpperRxIndication(uint8 controller, uint16 id, const uint8 *data, uint8 dlc)
{
    CanIf_RxIndication(controller, id, data, dlc);
}
void Can_UpperControllerBusOff(uint8 controller)
{
    CanIf_ControllerBusOff(controller);
}
