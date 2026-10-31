#ifndef DET_H
#define DET_H
#include "Std_Types.h"
#include <stdarg.h>

#define DET_MODULE_ID       ((uint16)15U)
#define DET_INSTANCE_ID     ((uint8)0U)

void Det_Init(void);
void Det_Start(void);
Std_ReturnType Det_ReportError(uint16 ModuleId, uint8 InstanceId, uint8 ApiId, uint8 ErrorId);
Std_ReturnType Det_ReportRuntimeError(uint16 ModuleId, uint8 InstanceId, uint8 ApiId, uint8 ErrorId);
void Det_Print(const char *s);
void Det_Printf(const char *fmt, ...);

#endif
