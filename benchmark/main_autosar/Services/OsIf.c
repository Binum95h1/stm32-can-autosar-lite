#include "OsIf.h"

static volatile uint32 g_tick_ms = 0U;

void SysTick_Handler(void)
{
    g_tick_ms++;
}

uint32 OsIf_GetTimeMs(void)
{
    return g_tick_ms;
}

void OsIf_DelayMs(uint32 ms)
{
    uint32 start = OsIf_GetTimeMs();
    while ((OsIf_GetTimeMs() - start) < ms) { __asm volatile ("nop"); }
}
