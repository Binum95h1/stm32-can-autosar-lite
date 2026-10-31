#include "BswM.h"

static BswM_StateType BswM_CurrentState = BSWM_STATE_UNINIT;

#define BSWM_START_SEC_CODE
#include "MemMap.h"

void BswM_Init(void)
{
    BswM_CurrentState = BSWM_STATE_STARTUP;
}

void BswM_RequestMode(BswM_StateType state)
{
    BswM_CurrentState = state;
}

BswM_StateType BswM_GetState(void)
{
    return BswM_CurrentState;
}

void BswM_MainFunction(void)
{
    /* Manual demo hook for future mode arbitration. */
}

#define BSWM_STOP_SEC_CODE
#include "MemMap.h"
