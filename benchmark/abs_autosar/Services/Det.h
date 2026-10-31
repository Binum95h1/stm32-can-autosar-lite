#ifndef SERVICES_DET_H
#define SERVICES_DET_H
#include "Std_Types.h"
void Det_Init(void);
void Det_Start(void);
void Det_Print(const char *s);
void Det_Printf(const char *format, ...);
Std_ReturnType Det_ReportError(uint16 ModuleId, uint8 InstanceId, uint8 ApiId, uint8 ErrorId);
Std_ReturnType Det_ReportRuntimeError(uint16 ModuleId, uint8 InstanceId, uint8 ApiId, uint8 ErrorId);
#endif
