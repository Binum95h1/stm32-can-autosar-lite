#ifndef HOST_MOCK_H
#define HOST_MOCK_H
#include "Std_Types.h"
#include "Can_Types.h"
#include "Mpu6050If.h"
#include <stdint.h>
typedef struct {uint32 when;uint16 id;uint8 dlc;uint8 data[8];uint8 ok;} MockFrame;
extern MockFrame mock_frames[320];
extern unsigned mock_n, mock_reads, mock_buzzer_edges, mock_delay_calls, mock_max_delay, mock_cfg_writes;
extern uint8 mock_last_config, mock_last_accel_config;
extern boolean mock_buzzer;
extern uint32 mock_now;
void mock_reset(void);
void mock_axes(sint16 x, sint16 y, sint16 z);
void mock_fail_sensor(boolean yes);
void mock_bad_whoami(boolean yes);
void mock_fail_tx(boolean yes);
void mock_advance(uint32 ms);
#endif
