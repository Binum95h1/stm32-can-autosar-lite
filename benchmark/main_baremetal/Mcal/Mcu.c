#include "Mcu.h"
#include "Reg.h"
/* 8 MHz external crystal (Blue Pill); HCLK 72, APB1 36, APB2 72 MHz. */
void SystemInit(void) { /* Reset startup calls before .data/.bss; no globals used. */ }
void nvic_enable(uint32_t irq)
{
    ((volatile uint32_t*)NVIC_ISER_BASE)[irq >> 5U] = 1UL << (irq & 31U);
}
void Mcu_InitClock(void)
{
    /* Flash prefetch + 2WS before increasing HCLK. */
    FLASH->ACR = FLASH_ACR_PRFTBE | FLASH_ACR_LATENCY_2WS;
    RCC->CR |= RCC_CR_HSEON;
    while ((RCC->CR & RCC_CR_HSERDY) == 0U) { __asm volatile("nop"); }
    /* PCLK1=HCLK/2, ADC clock=PCLK2/6 = 12 MHz; PLL=HSE*9=72 MHz. */
    RCC->CFGR = (4UL << 8) | (2UL << 14) | (1UL << 16) | (7UL << 18);
    RCC->CR |= RCC_CR_PLLON;
    while ((RCC->CR & RCC_CR_PLLRDY) == 0U) { __asm volatile("nop"); }
    RCC->CFGR = (RCC->CFGR & ~3UL) | RCC_CFGR_SW_PLL;
    while ((RCC->CFGR & (3UL << 2)) != RCC_CFGR_SWS_PLL) { __asm volatile("nop"); }
}
void Mcu_InitSysTick(void)
{
    SYSTICK->LOAD = 72000UL - 1UL;
    SYSTICK->VAL = 0UL;
    SYSTICK->CTRL = SYSTICK_CLKSRC | SYSTICK_TICKINT | SYSTICK_ENABLE;
}
