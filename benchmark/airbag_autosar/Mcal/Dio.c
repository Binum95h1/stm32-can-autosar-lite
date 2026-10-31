#include "Dio.h"
#include "Reg.h"
#include "Det.h"

#define RCC_APB2ENR_AFIOEN (1UL << 0)
#define RCC_APB2ENR_IOPAEN (1UL << 2)
#define RCC_APB2ENR_IOPBEN (1UL << 3)

static GPIO_t *Dio_GetPortPtr(Dio_PortType PortId)
{
    if (PortId == DIO_PORT_A) {
        return GPIOA;
    }
    if (PortId == DIO_PORT_B) {
        return GPIOB;
    }
    return NULL_PTR;
}

static GPIO_t *Dio_GetChannelPortPtr(Dio_ChannelType ChannelId, uint8 *BitPos)
{
    if (ChannelId < 16U) {
        *BitPos = (uint8)ChannelId;
        return GPIOA;
    }
    if (ChannelId < DIO_CHANNEL_COUNT) {
        *BitPos = (uint8)(ChannelId - 16U);
        return GPIOB;
    }
    return NULL_PTR;
}

void Dio_Init(const void *ConfigPtr)
{
    (void)ConfigPtr;

    RCC->APB2ENR |= RCC_APB2ENR_AFIOEN | RCC_APB2ENR_IOPAEN | RCC_APB2ENR_IOPBEN;

    /* PA2 USART2_TX AF push-pull, PA3 USART2_RX input floating. */
    GPIOA->CRL &= ~((0xFUL << 8) | (0xFUL << 12));
    GPIOA->CRL |=  ((0xBUL << 8) | (0x4UL << 12));

    /* PA10 buzzer output push-pull, PA11 input, PA12 CAN_TX AF push-pull. */
    GPIOA->CRH &= ~((0xFUL << 8) | (0xFUL << 12) | (0xFUL << 16));
    GPIOA->CRH |=  ((0x2UL << 8) | (0x4UL << 12) | (0xBUL << 16));
    GPIOA->BRR = (1UL << 10);

    /* CAN remap disabled: PA11 RX, PA12 TX. */
    AFIO->MAPR &= ~(0x3UL << 13);

    /* PB6/PB7 I2C1 AF open-drain 50 MHz. */
    GPIOB->CRL &= ~(0xFFUL << 24);
    GPIOB->CRL |=  (0xFUL << 24);
    GPIOB->CRL |=  (0xFUL << 28);
}

Dio_LevelType Dio_ReadChannel(Dio_ChannelType ChannelId)
{
    uint8 bitPos = 0U;
    GPIO_t *port = Dio_GetChannelPortPtr(ChannelId, &bitPos);

    if (port == NULL_PTR) {
        (void)Det_ReportError(DIO_MODULE_ID, DIO_INSTANCE_ID, DIO_SID_READ_CH, DIO_E_PARAM_INVALID_CHANNEL_ID);
        return STD_LOW;
    }

    return ((port->IDR & (1UL << bitPos)) != 0U) ? STD_HIGH : STD_LOW;
}

void Dio_WriteChannel(Dio_ChannelType ChannelId, Dio_LevelType Level)
{
    uint8 bitPos = 0U;
    GPIO_t *port = Dio_GetChannelPortPtr(ChannelId, &bitPos);

    if (port == NULL_PTR) {
        (void)Det_ReportError(DIO_MODULE_ID, DIO_INSTANCE_ID, DIO_SID_WRITE_CH, DIO_E_PARAM_INVALID_CHANNEL_ID);
        return;
    }

    if (Level == STD_HIGH) {
        port->BSRR = (1UL << bitPos);
    } else {
        port->BRR = (1UL << bitPos);
    }
}

Dio_PortLevelType Dio_ReadPort(Dio_PortType PortId)
{
    GPIO_t *port = Dio_GetPortPtr(PortId);

    if (port == NULL_PTR) {
        (void)Det_ReportError(DIO_MODULE_ID, DIO_INSTANCE_ID, DIO_SID_READ_PORT, DIO_E_PARAM_INVALID_PORT_ID);
        return 0U;
    }

    return (Dio_PortLevelType)(port->IDR & 0xFFFFU);
}

void Dio_WritePort(Dio_PortType PortId, Dio_PortLevelType Level)
{
    GPIO_t *port = Dio_GetPortPtr(PortId);

    if (port == NULL_PTR) {
        (void)Det_ReportError(DIO_MODULE_ID, DIO_INSTANCE_ID, DIO_SID_WRITE_PORT, DIO_E_PARAM_INVALID_PORT_ID);
        return;
    }

    port->ODR = (uint32)(Level & 0xFFFFU);
}

Dio_LevelType Dio_FlipChannel(Dio_ChannelType ChannelId)
{
    uint8 bitPos = 0U;
    Dio_LevelType newLevel;
    GPIO_t *port = Dio_GetChannelPortPtr(ChannelId, &bitPos);

    if (port == NULL_PTR) {
        (void)Det_ReportError(DIO_MODULE_ID, DIO_INSTANCE_ID, DIO_SID_FLIP_CH, DIO_E_PARAM_INVALID_CHANNEL_ID);
        return STD_LOW;
    }

    if ((port->ODR & (1UL << bitPos)) != 0U) {
        port->BRR = (1UL << bitPos);
        newLevel = STD_LOW;
    } else {
        port->BSRR = (1UL << bitPos);
        newLevel = STD_HIGH;
    }

    return newLevel;
}

void Dio_MaskedWritePort(Dio_PortType PortId, Dio_PortLevelType Level, Dio_PortLevelType Mask)
{
    GPIO_t *port = Dio_GetPortPtr(PortId);
    uint32 current;

    if (port == NULL_PTR) {
        (void)Det_ReportError(DIO_MODULE_ID, DIO_INSTANCE_ID, DIO_SID_WRITE_PORT, DIO_E_PARAM_INVALID_PORT_ID);
        return;
    }

    current = port->ODR;
    current &= ~((uint32)Mask);
    current |= ((uint32)Level & (uint32)Mask);
    port->ODR = current;
}

void Dio_InitPins(void)
{
    Dio_Init(NULL_PTR);
}

void Dio_WriteBuzzer(uint8 level)
{
    Dio_WriteChannel(DIO_CHANNEL_BUZZER, (level != 0U) ? STD_HIGH : STD_LOW);
}
