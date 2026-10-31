#include "OsIf.h"
#include "Mcu.h"

uint32 OsIf_GetTimeMs(void){
    return Mcu_GetTick();
}

void OsIf_DelayMs(uint32 ms){
    uint32 start = OsIf_GetTimeMs();
    while ((OsIf_GetTimeMs() - start) < ms){
        __asm volatile ("nop");
    }
}
