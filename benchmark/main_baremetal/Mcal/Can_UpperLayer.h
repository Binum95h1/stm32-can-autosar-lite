#ifndef CAN_UPPERLAYER_H
#define CAN_UPPERLAYER_H
#include "Std_Types.h"
void Can_UpperRxIndication(uint8 ControllerId, uint16 CanId, const uint8 *data, uint8 dlc);
void Can_UpperControllerBusOff(uint8 ControllerId);
#endif
