#include "Det.h"
#include "Uart.h"
#include <stdarg.h>
#include <stdio.h>

static char print_buf[180];
static boolean Det_Started = FALSE;

void Det_Init(void){
    Det_Started = FALSE;
}

void Det_Start(void){
    Det_Started = TRUE;
}

void Det_Print(const char *s){
    if (s == NULL_PTR) {
        return;
    }
    Uart_WriteString(s);
}

void Det_Printf(const char *format, ...){
    va_list args;
    int len;

    if (format == NULL_PTR){
        return;
    }

    va_start(args, format);
    len = vsnprintf(print_buf, sizeof(print_buf), format, args);
    va_end(args);

    if (len <= 0){
        return;
    }
    if (len >= (int)sizeof(print_buf)){
        len = (int)sizeof(print_buf) - 1;
    }

    for (int i = 0; i < len; i++) {
        if (Uart_SendByte((uint8)print_buf[i]) != E_OK) {
            return;
        }
    }
}

Std_ReturnType Det_ReportError(uint16 ModuleId, uint8 InstanceId, uint8 ApiId, uint8 ErrorId){
    (void)Det_Started;
    Det_Printf("DET DEV ERROR: module=%u instance=%u api=0x%02X error=0x%02X\r\n",
               (unsigned int)ModuleId,
               (unsigned int)InstanceId,
               (unsigned int)ApiId,
               (unsigned int)ErrorId);
    return E_OK;
}

Std_ReturnType Det_ReportRuntimeError(uint16 ModuleId, uint8 InstanceId, uint8 ApiId, uint8 ErrorId){
    Det_Printf("DET RUNTIME ERROR: module=%u instance=%u api=0x%02X error=0x%02X\r\n",
               (unsigned int)ModuleId,
               (unsigned int)InstanceId,
               (unsigned int)ApiId,
               (unsigned int)ErrorId);
    return E_OK;
}
