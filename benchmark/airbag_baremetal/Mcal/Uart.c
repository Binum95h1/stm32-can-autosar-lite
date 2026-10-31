#include "Uart.h"
#include "Reg.h"
#define RCC_APB1ENR_USART2EN (1UL << 17)
#define USART_SR_TXE (1UL << 7)
#define USART_CR1_UE (1UL << 13)
#define USART_CR1_TE (1UL << 3)
#define USART_CR1_RE (1UL << 2)
void Uart_Init(const Uart_ConfigType *ConfigPtr){
    (void)ConfigPtr;
    RCC->APB1ENR|=RCC_APB1ENR_USART2EN;
    (void)RCC->APB1ENR;
    USART2->CR1=0U;
    USART2->CR2=0U;
    USART2->CR3=0U;
    USART2->BRR=0x138U;
    USART2->CR1=USART_CR1_TE|USART_CR1_RE|USART_CR1_UE;
}
Std_ReturnType Uart_SendByte(uint8 byte){
    uint32 timeout=UART_TIMEOUT_CNT;
    while((USART2->SR&USART_SR_TXE)==0U){if(--timeout==0U)return E_NOT_OK;
    }
    USART2->DR=byte;
    return E_OK;
}
void Uart_Init_115200(void){
    Uart_Init(&Uart_Config);
} 
