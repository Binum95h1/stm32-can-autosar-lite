#include "EcuM.h"
#include "Mcu.h"
#include "Dio.h"
#include "Uart.h"
#include "I2c.h"
#include "Can.h"
#include "CanIf.h"
#include "PduR.h"
#include "ComIf.h"
#include "BswM.h"
#include "SchM.h"
#include "Det.h"
#include "OsIf.h"
#include "Mpu6050If.h"
#include "BuzzerIf.h"

#define ECUM_START_SEC_CODE
#include "MemMap.h"

void EcuM_Init(void)
{
    Mcu_Init(NULL_PTR);
    Mcu_InitSysTick();
    SchM_Init();
    Dio_Init(NULL_PTR);
    Uart_Init(&Uart_Config);
    Det_Init();
    Det_Start();
    I2c_Init(&I2c_Config);
    Can_Init(&Can_Config);
    (void)Can_SetControllerMode(CAN_MAIN_CONTROLLER, CAN_CS_STARTED);
    CanIf_Init(&CanIf_Config);
    PduR_Init(&PduR_Config);
    ComIf_Init();
    BswM_Init();
}

void EcuM_StartupTwo(void)
{
    Det_Print("\r\nManual AUTOSAR Classic-like STM32F103 Airbag Demo - Level 3-lite\r\n");
    (void)BuzzerIf_Set(TRUE);
    OsIf_DelayMs(100U);
    (void)BuzzerIf_Set(FALSE);
    OsIf_DelayMs(100U);

    if (Mpu6050If_Init() != E_OK) {
        Det_Printf("MPU6050 INIT FAIL | WHO_AM_I=0x%02X; periodic CAN remains active\r\n", Mpu6050If_GetWhoAmI());
    } else {
        Det_Printf("MPU6050 INIT OK | WHO_AM_I=0x%02X\r\n", Mpu6050If_GetWhoAmI());
    }
    BswM_RequestMode(BSWM_STATE_RUN);
}

void EcuM_MainFunction(void)
{
    BswM_MainFunction();
    CanIf_MainFunction();
    PduR_MainFunction();
}

#define ECUM_STOP_SEC_CODE
#include "MemMap.h"
