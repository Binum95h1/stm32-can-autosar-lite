#ifndef UART_H
#define UART_H
#include "Std_Types.h"
void Uart_Init(void);
Std_ReturnType Uart_SendByte(uint8 byte);
void Uart_SendBuffer(const char *buf, uint16 len);
void Uart_WriteString(const char *s);
void Uart_Printf(const char *fmt, ...);
#endif
