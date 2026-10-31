#include "Mcu.h"
#include "Dio.h"
#include "Uart.h"
#include "I2c.h"
#include "Can.h"
#include "Can_Cfg.h"
#include "SchM.h"
#include "Mpu6050If.h"
#include "BuzzerIf.h"
#include "BareCan.h"
#include "OsIf.h"
#include "AirbagS.h"
#include "AirbagS_Cfg.h"
#include "Det.h"
#include "BareCan_Cfg.h"

int main(void)
{
    uint32 next_tick;
    Mcu_Init(NULL_PTR);
    Mcu_InitSysTick();
    SchM_Init();
    Dio_Init(NULL_PTR);
    Uart_Init(&Uart_Config);
    Det_Init();
    Det_Start();
    I2c_Init(&I2c_Config);
    Can_Init(&Can_Config);
    (void)Can_SetControllerMode(CAN_MAIN_CONTROLLER,CAN_CS_STARTED);
    BareCan_Init();
    Det_Print("\r\nBare-metal STM32F103 Airbag Demo\r\n");
    (void)BuzzerIf_Set(TRUE);
    OsIf_DelayMs(100U);
    (void)BuzzerIf_Set(FALSE);
    OsIf_DelayMs(100U);
    if (Mpu6050If_Init() != E_OK) {
        Det_Printf("MPU6050 INIT FAIL | WHO_AM_I=0x%02X; periodic CAN remains active\r\n", Mpu6050If_GetWhoAmI());
    } else {
        Det_Printf("MPU6050 INIT OK | WHO_AM_I=0x%02X\r\n", Mpu6050If_GetWhoAmI());
    }

    Det_Printf("Threshold = %ld mg\r\n", (long)AIRBAGS_FORCE_THRESHOLD_MG);
    Det_Printf("[AIRBAG] CAN IDs: STATUS=0x%03lX DIAG=0x%03lX\r\n",
               (unsigned long)CAN_ID_AIRBAG_TO_MAIN,
               (unsigned long)CAN_ID_CAN_DIAG_STATUS);

    AirbagS_Init();
    next_tick = OsIf_GetTimeMs() + AIRBAGS_MAIN_PERIOD_MS;

    while (1) {
        Can_MainFunction_BusOff();
        Can_MainFunction_Mode();
        Can_MainFunction_Write();
        Can_MainFunction_Read();
        AirbagS_MainFunction();
        while ((sint32)(OsIf_GetTimeMs() - next_tick) < 0) { __asm volatile ("nop"); }
        do { next_tick += AIRBAGS_MAIN_PERIOD_MS; }
        while ((sint32)(OsIf_GetTimeMs() - next_tick) >= 0);
    }
}
