#include "Can_Cfg.h"

/* bxCAN 500 kbps, APB1 = 36 MHz: SJW=1TQ, BS1=9TQ, BS2=2TQ, Prescaler=6. */
const Can_ConfigType Can_Config = { 500000UL, 0x00180005UL };
