#ifndef COMIF_TYPES_H
#define COMIF_TYPES_H

#include "Std_Types.h"

typedef struct {
    uint8 wheel1Speed;
    uint8 wheel2Speed;
    uint8 motorState;
    uint8 sequence;
    uint8 dlc;
} ComIf_AbsFeedbackType;

typedef struct {
    uint8 deploy;
    uint8 sequence;
    uint8 reserved;
    uint8 dlc;
} ComIf_AirbagStatusType;

#endif /* COMIF_TYPES_H */
