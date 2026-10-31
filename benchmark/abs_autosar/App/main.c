#include <stdint.h>
#include "EcuM.h"
#include "OsIf.h"
#include "AbsNodeS.h"
#include "Project_Cfg.h"

int main(void)
{
    uint32_t next_tick;
    EcuM_Init();
    EcuM_StartupTwo();
    AbsNodeS_Init();
    next_tick = OsIf_GetTimeMs() + ABS_MAIN_PERIOD_MS;

    while (1) {
        EcuM_MainFunction();
        AbsNodeS_MainFunction();
        while ((int32_t)(OsIf_GetTimeMs() - next_tick) < 0) {
            __asm volatile ("nop");
        }
        do {
            next_tick += ABS_MAIN_PERIOD_MS;
        } while ((int32_t)(OsIf_GetTimeMs() - next_tick) >= 0);
    }
}

void NMI_Handler(void){
    while(1){}
}
void HardFault_Handler(void){
    while(1){}
}
void MemManage_Handler(void){
    while(1){}
}
void BusFault_Handler(void){
    while(1){}
}
void UsageFault_Handler(void){
    while(1){}
}
void SVC_Handler(void){}
void DebugMon_Handler(void){}
void PendSV_Handler(void){}
