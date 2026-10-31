#include "I2c.h"
#include "Reg.h"
#include "OsIf.h"
void I2c_Init(void)
{
    RCC->APB1ENR |= RCC_APB1ENR_I2C1EN;
    RCC->APB1RSTR |= RCC_APB1RSTR_I2C1RST;
    RCC->APB1RSTR &= ~RCC_APB1RSTR_I2C1RST;
    I2C1->CR1 = I2C_CR1_SWRST;
    I2C1->CR1 = 0U;
    I2C1->CR2 = 36U;    /* PCLK1 = 36MHz */
    I2C1->CCR = 180U;  /* Standard mode 100kHz: PCLK1 / (2*CCR) */
    I2C1->TRISE = 37U; /* 1000ns rise-time limit */
    I2C1->OAR1 = 1UL << 14;
    I2C1->CR1 = I2C_CR1_PE | I2C_CR1_ACK;
}
static uint8_t WaitSet(volatile uint32_t *reg, uint32_t mask)
{
    uint32_t n = I2C_TIMEOUT;
    while ((*reg & mask) == 0UL) if (--n == 0UL) return 0U;
    return 1U;
}
static uint8_t WaitClear(volatile uint32_t *reg, uint32_t mask)
{
    uint32_t n = I2C_TIMEOUT;
    while ((*reg & mask) != 0UL) if (--n == 0UL) return 0U;
    return 1U;
}
uint8_t I2c_MasterTransmit(uint8_t addr7, const uint8_t *data, uint8_t len)
{
    if (data == (void*)0 || len == 0U) return 0U;
    if (!WaitClear(&I2C1->SR2, I2C_SR2_BUSY)) return 0U;
    I2C1->CR1 |= I2C_CR1_START;
    if (!WaitSet(&I2C1->SR1, I2C_SR1_SB)) { I2C1->CR1 |= I2C_CR1_STOP; return 0U; }
    I2C1->DR = (uint32_t)(addr7 << 1);
    {
        uint32_t n = I2C_TIMEOUT;
        for (;;) {
            uint32_t sr = I2C1->SR1;
            if (sr & I2C_SR1_ADDR) break;
            if (sr & I2C_SR1_AF) {
                I2C1->SR1 &= ~I2C_SR1_AF;
                I2C1->CR1 |= I2C_CR1_STOP;
                return 0U;
            }
            if (--n == 0U) { I2C1->CR1 |= I2C_CR1_STOP; return 0U; }
        }
    }
    { volatile uint32_t tmp = I2C1->SR1; tmp = I2C1->SR2; (void)tmp; }
    for (uint8_t i = 0U; i < len; i++) {
        if (!WaitSet(&I2C1->SR1, I2C_SR1_TXE)) { I2C1->CR1 |= I2C_CR1_STOP; return 0U; }
        I2C1->DR = data[i];
    }
    if (!WaitSet(&I2C1->SR1, I2C_SR1_BTF)) { I2C1->CR1 |= I2C_CR1_STOP; return 0U; }
    I2C1->CR1 |= I2C_CR1_STOP;
    return 1U;
}
