#include <stdint.h>
#include "Mcu.h"
#include "Dio.h"
#include "Uart.h"
#include "Can.h"
#include "Can_Cfg.h"
#include "SchM.h"
#include "Det.h"
#include "MotorIf.h"
#include "BareCan.h"
#include "OsIf.h"
#include "AbsNodeS.h"
#include "Project_Cfg.h"

int main(void)
{
    uint32_t next_tick;
    /* Hardware / service initialization */
    Mcu_Init();
    SchM_Init();
    Dio_Init();
    Uart_Init();

    Det_Init();
    Det_Start();

    MotorIf_Init();

    /* CAN register-level driver */
    Can_Init(&Can_Config);
    (void)Can_SetControllerMode(
        CAN_MAIN_CONTROLLER,
        CAN_CS_STARTED
    );

    /* Bare-metal communication */
    BareCan_Init();

    Det_Print("\r\n");
    Det_Print("ABS Bare-metal STM32F103\r\n");
    Det_Print("CAN Driver: register-level bxCAN\r\n");
    Det_Print("BOOT OK\r\n");

    /* ABS application */
    AbsNodeS_Init();
    next_tick = OsIf_GetTimeMs() + ABS_MAIN_PERIOD_MS;

    while (1)
    {
        /*
         * Direct CAN driver polling.
         * No EcuM / BswM / CanIf / PduR / RTE.
         */
        Can_MainFunction_BusOff();
        Can_MainFunction_Mode();
        Can_MainFunction_Write();
        Can_MainFunction_Read();

        AbsNodeS_MainFunction();
        while ((int32_t)(OsIf_GetTimeMs() - next_tick) < 0) {
            __asm volatile ("nop");
        }
        do {
            next_tick += ABS_MAIN_PERIOD_MS;
        } while ((int32_t)(OsIf_GetTimeMs() - next_tick) >= 0);

        /*
         * NOTE:
         * AbsNodeS_MainFunction() hien tai da co
         * OsIf_DelayMs(5U) o cuoi ham.
         *
         * Khong delay them tai day de tranh double-delay.
         */
    }
}

void NMI_Handler(void)
{
    while (1) {}
}

void HardFault_Handler(void)
{
    while (1) {}
}

void MemManage_Handler(void)
{
    while (1) {}
}

void BusFault_Handler(void)
{
    while (1) {}
}

void UsageFault_Handler(void)
{
    while (1) {}
}

void SVC_Handler(void) {}
void DebugMon_Handler(void) {}
void PendSV_Handler(void) {}

