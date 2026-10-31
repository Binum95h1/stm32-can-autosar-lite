#include "Uart.h"
#include "Reg.h"
#include <stdarg.h>
#include <stdio.h>
static void Uart_WriteChar(char c){ 
    while((USART2->SR & USART_SR_TXE)==0U){} 
    USART2->DR=(uint32_t)(uint8_t)c; 
}
Std_ReturnType Uart_SendByte(uint8 byte)
{
    uint32 timeout = 100000U;
    while ((USART2->SR & USART_SR_TXE) == 0U) {
        if (--timeout == 0U) {
            return E_NOT_OK;
        }
    }
    USART2->DR = (uint32_t)byte;
    return E_OK;
}
void Uart_SendBuffer(const char *buf, uint16 len)
{
    if (buf == NULL_PTR) {
        return;
    }
    for (uint16 i = 0U; i < len; i++) {
        if (Uart_SendByte((uint8)buf[i]) != E_OK) {
            return;
        }
    }
}
void Uart_Init(void)
{
    RCC->APB2ENR |= RCC_APB2ENR_IOPAEN | RCC_APB2ENR_AFIOEN;
    RCC->APB1ENR |= RCC_APB1ENR_USART2EN;
    GPIOA->CRL &= ~((0xFUL<<(2U*4U)) | (0xFUL<<(3U*4U)));
    GPIOA->CRL |=  ((0xBUL<<(2U*4U)) | (0x4UL<<(3U*4U))); /* PA2 AF PP, PA3 floating input */
    USART2->BRR = 0x138UL; /* 36MHz / 115200 */
    USART2->CR1 = USART_CR1_UE | USART_CR1_TE | USART_CR1_RE;
}
void Uart_WriteString(const char *s){ 
    while(s && *s){ 
        Uart_WriteChar(*s++); 
    } 
}
void Uart_Printf(const char *fmt, ...)
{
    char b[160]; 
    va_list a; 
    va_start(a,fmt); 
    int n=vsnprintf(b,sizeof(b),fmt,a); 
    va_end(a);
    if(n>0){ 
        if(n>(int)sizeof(b)-1)n=sizeof(b)-1; 
        for(int i=0;i<n;i++)
        Uart_WriteChar(b[i]); 
    }
}
