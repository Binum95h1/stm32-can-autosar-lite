#include "Adc.h"
#include "Reg.h"
#include "OsIf.h"
#include "Det.h"
static uint32 Adc_LastValue = 0U;
static uint8 Adc_Ready = 0U;
static uint8 Adc_WaitClear(uint32_t mask)
{
    uint32_t n = ADC_TIMEOUT;
    while ((ADC1->CR2 & mask) != 0U) if (--n == 0U) return 0U;
    return 1U;
}
void Adc_Init(void)
{
    RCC->APB2ENR |= RCC_APB2ENR_ADC1EN;
    ADC1->CR1 = 0U;
    ADC1->CR2 = 0U;
    /* PA4 = ADC1_IN4; 239.5 cycles sample at ADC clock 12MHz. */
    ADC1->SMPR2 = (ADC1->SMPR2 & ~(7UL << 12)) | (7UL << 12);
    ADC1->SQR1 &= ~(15UL << 20); /* one regular channel */
    ADC1->SQR3 = (ADC1->SQR3 & ~31UL) | 4UL;
    /* F1 software triggering requires EXTSEL=111 and EXTTRIG=1. */
    ADC1->CR2 = (7UL << 17) | ADC_CR2_EXTTRIG | ADC_CR2_ADON;
    OsIf_DelayMs(1U);
    ADC1->CR2 |= ADC_CR2_RSTCAL;
    if (!Adc_WaitClear(ADC_CR2_RSTCAL)) return;
    ADC1->CR2 |= ADC_CR2_CAL;
    if (!Adc_WaitClear(ADC_CR2_CAL)) return;
    Adc_Ready = 1U;
}
uint32 Adc_Read(void)
{
    uint32_t n = ADC_TIMEOUT;
    if (Adc_Ready == 0U) return Adc_LastValue;
    ADC1->CR2 |= ADC_CR2_SWSTART;
    while ((ADC1->SR & ADC_SR_EOC) == 0U) {
        if (--n == 0U) {
            Det_Print("ADC TIMEOUT\r\n");
            return Adc_LastValue;
        }
    }
    Adc_LastValue = ADC1->DR & 0x0FFFUL;
    return Adc_LastValue;
}
