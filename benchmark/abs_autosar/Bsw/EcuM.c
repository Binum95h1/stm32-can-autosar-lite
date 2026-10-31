#include "EcuM.h"
#include "Mcu.h"
#include "Dio.h"
#include "Uart.h"
#include "Can.h"
#include "CanIf.h"
#include "PduR.h"
#include "ComIf.h"
#include "BswM.h"
#include "SchM.h"
#include "Det.h"
#include "MotorIf.h"
#include "Project_Cfg.h"

#define ECUM_START_SEC_CODE
#include "MemMap.h"

void EcuM_Init(void)
{
    Mcu_Init();
    SchM_Init();
    Dio_Init();
    Uart_Init();
    Det_Init();
    Det_Start();
    MotorIf_Init();
    Can_Init(&Can_Config);
    (void)Can_SetControllerMode(CAN_MAIN_CONTROLLER, CAN_CS_STARTED);
    CanIf_Init(&CanIf_Config);
    PduR_Init(&PduR_Config);
    ComIf_Init();
    BswM_Init();
}

void EcuM_StartupTwo(void)
{
    Det_Print("\r\nManual AUTOSAR Classic-like STM32F103 ABS Demo - Level 3-lite\r\n");
    Det_Printf("[ABS] CAN IDs: RX_MAIN=0x%03lX TX_MAIN=0x%03lX DIAG=0x%03lX\r\n",
               (unsigned long)CAN_ID_MAIN_TO_ABS,
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

#define ECUM_STOP_SEC_CODE
#include "MemMap.h"
