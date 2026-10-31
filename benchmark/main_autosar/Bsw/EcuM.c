#include "EcuM.h"
#include "Mcu.h"
#include "Dio.h"
#include "Uart.h"
#include "Adc.h"
#include "I2c.h"
#include "Can.h"
#include "CanIf.h"
#include "PduR.h"
#include "ComIf.h"
#include "BswM.h"
#include "SchM.h"
#include "Det.h"
#include "Project_Cfg.h"

void EcuM_Init(void)
{
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
    CanIf_Init(&CanIf_Config);
    PduR_Init(&PduR_Config);
    ComIf_Init();
    BswM_Init();
}

void EcuM_StartupTwo(void)
{
    Det_Print("\r\n=============================\r\n");
    Det_Print("Manual AUTOSAR Classic-like STM32F103C8 Main Demo - Level 3-lite\r\n");
    Det_Printf("[MAIN] CAN IDs: TX_ABS=0x%03lX RX_AIRBAG=0x%03lX RX_ABS=0x%03lX DIAG=0x%03lX\r\n",
               (unsigned long)CAN_ID_MAIN_TO_ABS,
               (unsigned long)CAN_ID_AIRBAG_TO_MAIN,
               (unsigned long)CAN_ID_ABS_TO_MAIN,
               (unsigned long)CAN_ID_CAN_DIAG_STATUS);
    Det_Print("BOOT OK\r\n");
    Det_Print("UART2 OK\r\n");
    Det_Print("CAN Filter Config OK\r\n");
    Det_Print("CAN Start OK\r\n");
    Det_Print("CAN Notification OK\r\n");
    BswM_RequestMode(BSWM_STATE_RUN);
}

void EcuM_MainFunction(void)
{
    BswM_MainFunction();
    CanIf_MainFunction();
    PduR_MainFunction();
}
