#ifndef MCAL_UART_H
#define MCAL_UART_H
#include <stdint.h>
#include "Std_Types.h"
void Uart_Init(void);
void Uart_SendByte(uint8_t c);
void Uart_SendBuffer(const char *s, uint16_t len);
#endif
