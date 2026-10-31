#include "Mcu.h"
#include "Reg.h"
static volatile uint32_t g_tick;
void SystemInit(void) { }
void SysTick_Handler(void){ 
    g_tick++; 
}
uint32_t Mcu_GetTick(void){ 
    return g_tick; 
}
void Mcu_Delay(uint32_t ms){ 
    uint32_t s = g_tick; 
    while ((g_tick - s) < ms) { 
        __asm volatile("nop"); 
    } 
}
void Mcu_Init(void){
    RCC->CR |= (1UL << 16); 
    while ((RCC->CR & (1UL << 17)) == 0U) { }

    FLASH->ACR = (1UL << 4) | (2UL << 0); /* PRFTBE + 2 wait states */
    RCC->CFGR = 0; RCC->CFGR |= (4UL << 8); /* PCLK1 = HCLK/2 */
    RCC->CFGR |= (7UL << 18); /* PLL x9 */
    RCC->CFGR |= (1UL << 16); /* PLLSRC HSE */
    RCC->CR |= (1UL << 24); 
    while ((RCC->CR & (1UL << 25)) == 0U) { }

    RCC->CFGR |= (2UL << 0); 
    while (((RCC->CFGR >> 2) & 3UL) != 2UL) { }

    SYSTICK->LOAD = 72000UL - 1UL; SYSTICK->VAL = 0; SYSTICK->CTRL = 7UL;
}
