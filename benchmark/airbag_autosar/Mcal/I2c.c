#include "I2c.h"
#include "Reg.h"
#include "../Config/Project_Cfg.h"
#define RCC_APB1ENR_I2C1EN (1UL << 21)
#define RCC_APB1RSTR_I2C1RST (1UL << 21)
#define I2C_CR1_PE (1UL << 0)
#define I2C_CR1_START (1UL << 8)
#define I2C_CR1_STOP (1UL << 9)
#define I2C_CR1_ACK (1UL << 10)
#define I2C_CR1_POS (1UL << 11)
#define I2C_CR1_SWRST (1UL << 15)
#define I2C_SR1_SB (1UL << 0)
#define I2C_SR1_ADDR (1UL << 1)
#define I2C_SR1_BTF (1UL << 2)
#define I2C_SR1_RXNE (1UL << 6)
#define I2C_SR1_TXE (1UL << 7)
#define I2C_SR1_BERR (1UL << 8)
#define I2C_SR1_ARLO (1UL << 9)
#define I2C_SR1_AF (1UL << 10)
#define I2C_SR1_OVR (1UL << 11)
#define I2C_SR2_BUSY (1UL << 1)

static uint8_t I2C_WaitSet(volatile uint32_t *reg, uint32_t mask)
{
    uint32_t timeout = I2C_TIMEOUT_CNT;
    while (((*reg) & mask) == 0U) {
        if (--timeout == 0U) return 0U;
    }
    return 1U;
}
static uint8_t I2C_WaitClear(volatile uint32_t *reg, uint32_t mask)
{
    uint32_t timeout = I2C_TIMEOUT_CNT;
    while (((*reg) & mask) != 0U) {
        if (--timeout == 0U) return 0U;
    }
    return 1U;
}
static void I2C_ClearADDR(void)
{
    volatile uint32_t tmp;
    tmp = I2C1->SR1;
    tmp = I2C1->SR2;
    (void)tmp;
}
static void I2C_ClearErrors(void)
{
    I2C1->SR1 &= ~(I2C_SR1_BERR | I2C_SR1_ARLO | I2C_SR1_AF | I2C_SR1_OVR);
}

static uint8_t I2C_Start(void)
{
    I2C1->CR1 |= I2C_CR1_START;
    return I2C_WaitSet(&I2C1->SR1, I2C_SR1_SB);
}

static void I2C_Stop(void)
{
    I2C1->CR1 |= I2C_CR1_STOP;
}
static uint8_t I2C_SendAddress(uint8_t addr7, uint8_t read)
{
    uint32_t timeout = I2C_TIMEOUT_CNT;

    I2C1->DR = (uint8_t)((addr7 << 1) | (read ? 1U : 0U));

    while ((I2C1->SR1 & I2C_SR1_ADDR) == 0U) {
        if (I2C1->SR1 & I2C_SR1_AF) {
            I2C1->SR1 &= ~I2C_SR1_AF;
            I2C_Stop();
            return 0U;
        }
        if (--timeout == 0U) {
            I2C_Stop();
            return 0U;
        }
    }
    return 1U;
}

static uint8_t I2C_WriteByte(uint8_t byte)
{
    if (!I2C_WaitSet(&I2C1->SR1, I2C_SR1_TXE)) return 0U;
    I2C1->DR = byte;
    return 1U;
}

void I2C_Init_100kHz(void)
{
    RCC->APB1ENR |= RCC_APB1ENR_I2C1EN;
    (void)RCC->APB1ENR;

    RCC->APB1RSTR |=  RCC_APB1RSTR_I2C1RST;
    RCC->APB1RSTR &= ~RCC_APB1RSTR_I2C1RST;

    I2C1->CR1 &= ~I2C_CR1_PE;

    I2C1->CR1 |= I2C_CR1_SWRST;
    I2C1->CR1 &= ~I2C_CR1_SWRST;

    I2C1->CR2   = 36U;
    I2C1->CCR   = 180U;
    I2C1->TRISE = 37U;
    I2C1->OAR1  = (1UL << 14);

    I2C1->CR1 |= I2C_CR1_PE;
    I2C1->CR1 |= I2C_CR1_ACK;
}

uint8_t I2C_WriteReg(uint8_t addr7, uint8_t reg, uint8_t value)
{
    if (!I2C_WaitClear(&I2C1->SR2, I2C_SR2_BUSY)) return 0U;
    I2C_ClearErrors();

    if (!I2C_Start()) return 0U;
    if (!I2C_SendAddress(addr7, 0U)) return 0U;
    I2C_ClearADDR();

    if (!I2C_WriteByte(reg)) return 0U;
    if (!I2C_WriteByte(value)) return 0U;
    if (!I2C_WaitSet(&I2C1->SR1, I2C_SR1_BTF)) return 0U;

    I2C_Stop();
    return 1U;
}

uint8_t I2C_ReadReg(uint8_t addr7, uint8_t reg, uint8_t *value)
{
    if (value == 0) return 0U;
    return I2C_ReadRegMulti(addr7, reg, value, 1U);
}

uint8_t I2C_ReadRegMulti(uint8_t addr7, uint8_t reg, uint8_t *data, uint8_t len)
{
    if ((data == 0) || (len == 0U)) return 0U;

    if (!I2C_WaitClear(&I2C1->SR2, I2C_SR2_BUSY)) return 0U;
    I2C_ClearErrors();

    if (!I2C_Start()) return 0U;
    if (!I2C_SendAddress(addr7, 0U)) return 0U;
    I2C_ClearADDR();

    if (!I2C_WriteByte(reg)) return 0U;
    if (!I2C_WaitSet(&I2C1->SR1, I2C_SR1_BTF)) return 0U;

    if (len == 1U) {
        I2C1->CR1 &= ~I2C_CR1_ACK;
        I2C1->CR1 &= ~I2C_CR1_POS;

        if (!I2C_Start()) return 0U;
        if (!I2C_SendAddress(addr7, 1U)) return 0U;

        __asm volatile ("cpsid i");
        I2C_ClearADDR();
        I2C_Stop();
        __asm volatile ("cpsie i");

        if (!I2C_WaitSet(&I2C1->SR1, I2C_SR1_RXNE)) return 0U;
        data[0] = (uint8_t)I2C1->DR;
    }
    else if (len == 2U) {
        I2C1->CR1 &= ~I2C_CR1_ACK;
        I2C1->CR1 |=  I2C_CR1_POS;

        if (!I2C_Start()) return 0U;
        if (!I2C_SendAddress(addr7, 1U)) return 0U;

        __asm volatile ("cpsid i");
        I2C_ClearADDR();
        __asm volatile ("cpsie i");

        if (!I2C_WaitSet(&I2C1->SR1, I2C_SR1_BTF)) return 0U;

        __asm volatile ("cpsid i");
        I2C_Stop();
        data[0] = (uint8_t)I2C1->DR;
        __asm volatile ("cpsie i");
        data[1] = (uint8_t)I2C1->DR;
    }
    else {
        I2C1->CR1 |=  I2C_CR1_ACK;
        I2C1->CR1 &= ~I2C_CR1_POS;

        if (!I2C_Start()) return 0U;
        if (!I2C_SendAddress(addr7, 1U)) return 0U;
        I2C_ClearADDR();

        uint8_t remaining = len;
        uint8_t index = 0U;

        while (remaining > 3U) {
            if (!I2C_WaitSet(&I2C1->SR1, I2C_SR1_RXNE)) return 0U;
            data[index++] = (uint8_t)I2C1->DR;
            remaining--;
        }

        if (!I2C_WaitSet(&I2C1->SR1, I2C_SR1_BTF)) return 0U;
        I2C1->CR1 &= ~I2C_CR1_ACK;

        __asm volatile ("cpsid i");
        data[index++] = (uint8_t)I2C1->DR;
        remaining--;
        I2C_Stop();
        __asm volatile ("cpsie i");

        data[index++] = (uint8_t)I2C1->DR;
        remaining--;

        if (!I2C_WaitSet(&I2C1->SR1, I2C_SR1_RXNE)) return 0U;
        data[index++] = (uint8_t)I2C1->DR;
        remaining--;

        (void)remaining;
    }

    I2C1->CR1 |= I2C_CR1_ACK;
    I2C1->CR1 &= ~I2C_CR1_POS;
    return 1U;
}
void I2c_Init(const I2c_ConfigType *ConfigPtr)
{
    (void)ConfigPtr;
    I2C_Init_100kHz();
}

Std_ReturnType I2c_WriteReg(uint8 addr7, uint8 reg, uint8 value)
{
    return (I2C_WriteReg(addr7, reg, value) != 0U) ? E_OK : E_NOT_OK;
}

Std_ReturnType I2c_ReadReg(uint8 addr7, uint8 reg, uint8 *value)
{
    return (I2C_ReadReg(addr7, reg, value) != 0U) ? E_OK : E_NOT_OK;
}

Std_ReturnType I2c_ReadRegMulti(uint8 addr7, uint8 reg, uint8 *data, uint8 len)
{
    return (I2C_ReadRegMulti(addr7, reg, data, len) != 0U) ? E_OK : E_NOT_OK;
}
