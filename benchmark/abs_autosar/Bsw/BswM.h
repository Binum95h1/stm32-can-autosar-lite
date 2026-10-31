#ifndef BSWM_H
#define BSWM_H

#include "Std_Types.h"
#include "BswM_Types.h"

void BswM_Init(void);
void BswM_RequestMode(BswM_StateType state);
BswM_StateType BswM_GetState(void);
void BswM_MainFunction(void);

#endif /* BSWM_H */
