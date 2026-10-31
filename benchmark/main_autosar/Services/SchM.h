#ifndef SCHM_H
#define SCHM_H

#include "Std_Types.h"

void SchM_Init(void);
void SchM_Enter_Can_EXCLUSIVE_AREA_0(void);
void SchM_Exit_Can_EXCLUSIVE_AREA_0(void);
void SchM_Enter_CanIf_EXCLUSIVE_AREA_0(void);
void SchM_Exit_CanIf_EXCLUSIVE_AREA_0(void);

#endif /* SCHM_H */
