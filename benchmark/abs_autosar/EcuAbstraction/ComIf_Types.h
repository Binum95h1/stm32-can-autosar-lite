#ifndef COMIF_TYPES_H
#define COMIF_TYPES_H

#include "Std_Types.h"

typedef struct {
    uint16 adcRaw;
    uint8 brake;
    uint8 direction;
    uint8 sequence;
    uint8 dlc;
} ComIf_MainCommandType;

#endif /* COMIF_TYPES_H */
