#include "EcuM.h"
#include "MainNodeS.h"
#include "OsIf.h"
#include "Det.h"
#include "Project_Cfg.h"

int main(void)
{
    uint32 next_tick;
    EcuM_Init();
    EcuM_StartupTwo();
    MainNodeS_Init();

    Det_Printf("MAIN READY | TX_ABS:0x121 | RX_AIRBAG:0x111 | RX_ABS:0x122 | CAN_DIAG:0x211\r\n");
    Det_Printf("=============================\r\n\r\n");

    next_tick = OsIf_GetTimeMs() + MAIN_TASK_PERIOD_MS;
    while (1) {
        EcuM_MainFunction();
        MainNodeS_MainFunction();
        while ((int32_t)(OsIf_GetTimeMs() - next_tick) < 0) {
            __asm volatile ("nop");
        }
        do {
            next_tick += MAIN_TASK_PERIOD_MS;
        } while ((int32_t)(OsIf_GetTimeMs() - next_tick) >= 0);
    }
}
