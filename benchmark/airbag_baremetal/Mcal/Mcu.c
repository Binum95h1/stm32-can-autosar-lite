#include "Mcu.h"
#include "Reg.h"

#define RCC_CR_HSEON         (1UL << 16)
#define RCC_CR_HSERDY        (1UL << 17)
#define RCC_CR_PLLON         (1UL << 24)
#define RCC_CR_PLLRDY        (1UL << 25)
#define RCC_CFGR_SW_PLL      (0x2UL << 0)
#define RCC_CFGR_SWS_PLL     (0x2UL << 2)
#define RCC_CFGR_HPRE_DIV1   (0x0UL << 4)
#define RCC_CFGR_PPRE1_DIV2  (0x4UL << 8)
#define RCC_CFGR_PPRE2_DIV1  (0x0UL << 11)
#define RCC_CFGR_PLLSRC_HSE  (1UL << 16)
#define RCC_CFGR_PLLMUL9     (0x7UL << 18)
#define FLASH_ACR_LATENCY_2  0x2UL
#define FLASH_ACR_PRFTBE     (1UL << 4)
#define SYSTICK_CTRL_ENABLE  (1UL << 0)
#define SYSTICK_CTRL_TICKINT (1UL << 1)
#define SYSTICK_CTRL_CLKSRC  (1UL << 2)
#define AIRCR                (*(volatile uint32 *)0xE000ED0CUL)
#define AIRCR_VECTKEY        (0x5FAUL << 16)
#define AIRCR_SYSRESETREQ    (1UL << 2)

static volatile uint32 g_tick_ms;
static const Mcu_ConfigType Mcu_DefaultConfig = { 72000000UL, 72000UL - 1UL };
static const Mcu_ConfigType *Mcu_ConfigPtr = &Mcu_DefaultConfig;

void SysTick_Handler(void)
{
    g_tick_ms++;
}

uint32 Mcu_GetTickMs(void)
{
    return g_tick_ms;
}

void Mcu_Init(const Mcu_ConfigType *ConfigPtr)
{
    Mcu_ConfigPtr = (ConfigPtr != NULL_PTR) ? ConfigPtr : &Mcu_DefaultConfig;
    Mcu_InitClock();
}

void Mcu_InitClock(void)
{
    FLASH->ACR = FLASH_ACR_LATENCY_2 | FLASH_ACR_PRFTBE;

    RCC->CR |= RCC_CR_HSEON;
    while ((RCC->CR & RCC_CR_HSERDY) == 0U) { }

    RCC->CFGR = RCC_CFGR_PLLSRC_HSE |
                RCC_CFGR_PLLMUL9    |
                RCC_CFGR_HPRE_DIV1  |
                RCC_CFGR_PPRE1_DIV2 |
                RCC_CFGR_PPRE2_DIV1;

    RCC->CR |= RCC_CR_PLLON;
    while (Mcu_GetPllStatus() != MCU_PLL_LOCKED) { }

    RCC->CFGR |= RCC_CFGR_SW_PLL;
    while ((RCC->CFGR & (0x3UL << 2)) != RCC_CFGR_SWS_PLL) { }
}

void Mcu_InitSysTick(void)
{
    SYSTICK->LOAD = Mcu_ConfigPtr->systickReload;
    SYSTICK->VAL = 0UL;
    SYSTICK->CTRL = SYSTICK_CTRL_CLKSRC | SYSTICK_CTRL_TICKINT | SYSTICK_CTRL_ENABLE;
}

Mcu_PllStatusType Mcu_GetPllStatus(void)
{
    return ((RCC->CR & RCC_CR_PLLRDY) != 0U) ? MCU_PLL_LOCKED : MCU_PLL_UNLOCKED;
}

Mcu_ResetType Mcu_GetResetReason(void)
{
    return MCU_RESET_UNDEFINED;
}

Mcu_RawResetType Mcu_GetResetRawValue(void)
{
    return 0U;
}

void Mcu_PerformReset(void)
{
    AIRCR = AIRCR_VECTKEY | AIRCR_SYSRESETREQ;
    while (1) { }
}
