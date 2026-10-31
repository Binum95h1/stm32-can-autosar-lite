#include "Det.h"
#include "Uart.h"
#include <stdarg.h>
#include <stdio.h>
#include <string.h>

static char Det_Buffer[256];

void Det_Init(void)
{
    (void)memset(Det_Buffer, 0, sizeof(Det_Buffer));
}

void Det_Start(void)
{
    /* No state machine needed in this educational DET. */
}

Std_ReturnType Det_ReportError(uint16 ModuleId, uint8 InstanceId, uint8 ApiId, uint8 ErrorId)
{
    Det_Printf("[DET] MOD=%u INST=%u API=0x%02X ERR=0x%02X\r\n",
               ModuleId, InstanceId, ApiId, ErrorId);
    return E_OK;
}

void Det_Print(const char *s)
{
    if (s == NULL_PTR) {
        return;
    }
    Uart_SendBuffer(s, (uint16)strlen(s));
}

void Det_Printf(const char *format, ...)
{
    va_list args;
    int len;
    uint16 tx_len;

    if (format == NULL_PTR) {
        return;
    }

    (void)memset(Det_Buffer, 0, sizeof(Det_Buffer));
    va_start(args, format);
    len = vsnprintf(Det_Buffer, sizeof(Det_Buffer), format, args);
    va_end(args);

    if (len > 0) {
        tx_len = (len >= (int)sizeof(Det_Buffer)) ? (uint16)(sizeof(Det_Buffer) - 1U) : (uint16)len;
        Uart_SendBuffer(Det_Buffer, tx_len);
    }
}
