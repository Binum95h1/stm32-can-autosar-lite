#ifndef ABS_HOST_SIM_H
#define ABS_HOST_SIM_H
#include <stdint.h>
#include "Can_Types.h"
#define SIM_MAX_FRAMES 256
#define SIM_LOG_CAP 16384
typedef struct {uint32_t ms;uint16_t id;uint8_t dlc;uint8_t payload[8];} SimFrame;
typedef struct {
    uint32_t now_ms;
    uint16_t pwm[2];
    uint8_t dir[2][2];
    uint16_t encoder[2];
    float encoder_fraction[2];
    float pulse_rate[2];
    int motor_stalled;
    int fail_next_tx;
    int dropped_tx;
    int frame_count;
    SimFrame frames[SIM_MAX_FRAMES];
    Can_DebugStatusType diag;
    char log[SIM_LOG_CAP];
    uint32_t log_length;
} SimState;
extern SimState sim;
void sim_reset(void);
void sim_advance_10ms(void);
#endif
