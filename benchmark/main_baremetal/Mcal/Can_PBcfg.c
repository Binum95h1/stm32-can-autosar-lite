#include "Can_Cfg.h"
/* STM32F103: PCLK1 = 36 MHz, prescaler=6, BS1=9, BS2=2, SJW=1
 * CAN bitrate = 36000000 / (6 * (1 + 9 + 2)) = 500000 bit/s.
 */
const Can_ConfigType Can_Config = { 500000UL, 0x00180005UL };
