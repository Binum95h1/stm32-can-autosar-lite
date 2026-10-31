#include "Dio.h"
#include "Reg.h"
/* F103C8T6 LQFP48 pin map: PA4 ADC, PA2/PA3 USART2,
 * PB6/PB7 I2C1, PA11/PA12 CAN1, PB0 brake, PB1 direction.
 */
void Dio_Init(void)
{
    RCC->APB2ENR |= RCC_APB2ENR_AFIOEN | RCC_APB2ENR_IOPAEN | RCC_APB2ENR_IOPBEN;
    /* CAN default AFIO mapping PA11 RX / PA12 TX; USART2 and I2C1 default. */
    AFIO->MAPR &= ~(3UL << 13);
    /* GPIOA CRL: PA2 alternate PP 50MHz (0xB), PA3 floating input (0x4), PA4 analog (0x0). */
    GPIOA->CRL &= ~((15UL << 8) | (15UL << 12) | (15UL << 16));
    GPIOA->CRL |=  (11UL << 8) | (4UL << 12);
    /* GPIOA CRH: PA11 CAN RX floating input, PA12 CAN TX alternate PP 50MHz. */
    GPIOA->CRH &= ~((15UL << 12) | (15UL << 16));
    GPIOA->CRH |=  (4UL << 12) | (11UL << 16);
    /* GPIOB CRL: PB0/PB1 input pull-up, PB6/PB7 I2C AF open drain 50MHz. */
    GPIOB->CRL &= ~((15UL << 0) | (15UL << 4) | (15UL << 24) | (15UL << 28));
    GPIOB->CRL |=  (8UL << 0) | (8UL << 4) | (15UL << 24) | (15UL << 28);
    GPIOB->BSRR = (1UL << 0) | (1UL << 1); /* pull-up, active LOW buttons */
}
uint8 Dio_ReadChannel(Dio_ChannelType ChannelId)
{
    switch (ChannelId) {
    case DIO_CHANNEL_BRAKE_SW:
        return ((GPIOB->IDR & 1UL) == 0UL) ? 1U : 0U;
    case DIO_CHANNEL_DIRECTION_SW:
        return ((GPIOB->IDR & 2UL) == 0UL) ? 1U : 0U;
    default: return 0U;
    }
}
