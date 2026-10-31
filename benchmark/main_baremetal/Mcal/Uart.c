#include "Uart.h"
#include "Reg.h"
void Uart_Init(void)
{
    RCC->APB1ENR |= RCC_APB1ENR_USART2EN;
    USART2->CR1 = 0U;
    USART2->CR2 = 0U;
    USART2->CR3 = 0U;
    USART2->BRR = 0x138UL; /* USART2 PCLK1=36MHz, 115200 baud */
    USART2->CR1 = USART_CR1_TE | USART_CR1_RE | USART_CR1_UE;
}
void Uart_SendByte(uint8_t c)
{
    uint32_t cnt = UART_TIMEOUT;
    while ((USART2->SR & USART_SR_TXE) == 0U) {
        if (--cnt == 0U) return;
    }
    USART2->DR = c;
}
void Uart_SendBuffer(const char *s, uint16_t len)
{
    for (uint16_t i = 0U; i < len; i++) Uart_SendByte((uint8_t)s[i]);
    uint32_t cnt = UART_TIMEOUT;
    while ((USART2->SR & USART_SR_TC) == 0U) {
        if (--cnt == 0U) break;
    }
}
