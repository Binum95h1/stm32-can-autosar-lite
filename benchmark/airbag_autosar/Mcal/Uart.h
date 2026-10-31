#ifndef UART_H
#define UART_H
#include "Std_Types.h"
#include "Uart_Types.h"
#include "Uart_Cfg.h"
void Uart_Init(const Uart_ConfigType *ConfigPtr);
Std_ReturnType Uart_SendByte(uint8 byte);
void Uart_Init_115200(void);
#endif
