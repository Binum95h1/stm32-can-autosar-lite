#include "EcuM.h"
#include "OsIf.h"
#include "AirbagS.h"
#include "AirbagS_Cfg.h"
#include "Det.h"
#include "ComIf_Cfg.h"

int main(void)
{
    uint32 next_tick;
    EcuM_Init();
    EcuM_StartupTwo();

    Det_Printf("Threshold = %ld mg\r\n", (long)AIRBAGS_FORCE_THRESHOLD_MG);
    Det_Printf("[AIRBAG] CAN IDs: STATUS=0x%03lX DIAG=0x%03lX\r\n",
               (unsigned long)CAN_ID_AIRBAG_TO_MAIN,
               (unsigned long)CAN_ID_CAN_DIAG_STATUS);

    AirbagS_Init();
    next_tick = OsIf_GetTimeMs() + AIRBAGS_MAIN_PERIOD_MS;

    while (1) {
        EcuM_MainFunction();
        AirbagS_MainFunction();
        while ((sint32)(OsIf_GetTimeMs() - next_tick) < 0) { __asm volatile ("nop"); }
        do { next_tick += AIRBAGS_MAIN_PERIOD_MS; }
        while ((sint32)(OsIf_GetTimeMs() - next_tick) >= 0);
    }
}
