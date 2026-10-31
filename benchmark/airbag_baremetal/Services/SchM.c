#include "SchM.h"

#define SCHM_START_SEC_CODE
#include "MemMap.h"

void SchM_Init(void)
{
    /* Manual Classic-like demo: no OS, so exclusive areas are IRQ gates. */
}

void SchM_Enter_Can_EXCLUSIVE_AREA_0(void)
{
#if defined(__arm__) || defined(__thumb__)
    __asm volatile ("cpsid i" : : : "memory");
#endif
}

void SchM_Exit_Can_EXCLUSIVE_AREA_0(void)
{
#if defined(__arm__) || defined(__thumb__)
    __asm volatile ("cpsie i" : : : "memory");
#endif
}

void SchM_Enter_CanIf_EXCLUSIVE_AREA_0(void)
{
#if defined(__arm__) || defined(__thumb__)
    __asm volatile ("cpsid i" : : : "memory");
#endif
}

void SchM_Exit_CanIf_EXCLUSIVE_AREA_0(void)
{
#if defined(__arm__) || defined(__thumb__)
    __asm volatile ("cpsie i" : : : "memory");
#endif
}

#define SCHM_STOP_SEC_CODE
#include "MemMap.h"
