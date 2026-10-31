#ifndef ABS_NODE_S_H
#define ABS_NODE_S_H
#include <stdint.h>
void AbsNodeS_Init(void);
void AbsNodeS_MainFunction(void);
void AbsNodeS_CanRxIndication(uint32_t std_id, const uint8_t *data, uint8_t dlc);
#endif
