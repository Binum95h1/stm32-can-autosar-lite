#ifndef PROJECT_CFG_H
#define PROJECT_CFG_H

#include <stdint.h>

#define SYSCLK_HZ                 72000000UL
#define APB1_CLK_HZ               36000000UL

#define FORCE_THRESHOLD_MG        2500L
#define FORCE_THRESHOLD_SQ        (FORCE_THRESHOLD_MG * FORCE_THRESHOLD_MG)

#define I2C_TIMEOUT_CNT           100000UL
#define UART_TIMEOUT_CNT          100000UL
#define CAN_TIMEOUT_CNT           100000UL
#define CAN_ID_AIRBAG_TO_MAIN     0x111UL

#endif
