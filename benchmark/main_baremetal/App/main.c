#include "Mcu.h"
#include "SchM.h"
#include "Dio.h"
#include "Uart.h"
#include "Det.h"
#include "Adc.h"
#include "I2c.h"
#include "Can.h"
#include "Can_Cfg.h"
#include "BareCan.h"
#include "OsIf.h"
#include "MainNodeS.h"
#include "Project_Cfg.h"
#include <stdint.h>
int main(void)
{
    uint32 next_tick;
    Mcu_InitClock();
    Mcu_InitSysTick();
    SchM_Init();
    Dio_Init();
    Uart_Init();
    Det_Init();
    Det_Start();
    Adc_Init();
    I2c_Init();
    Can_Init(&Can_Config);
    (void)Can_SetControllerMode(CAN_MAIN_CONTROLLER, CAN_CS_STARTED);
    BareCan_Init();
    MainNodeS_Init();
    Det_Print("MAIN BARE READY | TX_ABS:0x121 | RX_AIRBAG:0x111 | RX_ABS:0x122 | DIAG:0x211\r\n");
    next_tick = OsIf_GetTimeMs() + MAIN_TASK_PERIOD_MS;
    while (1) {
        Can_MainFunction_BusOff();
        Can_MainFunction_Mode();
        Can_MainFunction_Write();
        Can_MainFunction_Read();
        MainNodeS_MainFunction();
        while ((int32_t)(OsIf_GetTimeMs() - next_tick) < 0) {
            __asm volatile ("nop");
        }
        do {
            next_tick += MAIN_TASK_PERIOD_MS;
        } while ((int32_t)(OsIf_GetTimeMs() - next_tick) >= 0);
    }
}
