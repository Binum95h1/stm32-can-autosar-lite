#ifndef HOST_MOCK_H
#define HOST_MOCK_H
#include "Can_Types.h"
#include "Std_Types.h"
typedef struct {uint16 id; uint8 dlc; uint8 data[8];} MockFrame;
extern uint32 mock_ms;
extern uint32 mock_adc;
extern uint8 mock_brake;
extern uint8 mock_direction;
extern int mock_fail_next_tx;
extern MockFrame mock_frames[64];
extern int mock_frame_count;
void Mock_Reset(void);
void Mock_Inject(uint16 id, const uint8 *data, uint8 dlc);
#endif
