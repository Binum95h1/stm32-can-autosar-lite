#ifndef ECUABSTRACTION_LCDIF_H
#define ECUABSTRACTION_LCDIF_H
#include <stdint.h>
#include "Std_Types.h"
void LcdIf_Init(void);
void LcdIf_Clear(void);
void LcdIf_SetCursor(int row, int col);
void LcdIf_SendString(const char *str);
#endif
