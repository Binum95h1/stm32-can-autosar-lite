#ifndef PROJECT_CFG_H
#define PROJECT_CFG_H

#define SYSCLK_HZ                 72000000UL
#define APB1_CLK_HZ               36000000UL

#define CAN_ID_MAIN_TO_ABS        0x121U
#define CAN_ID_ABS_TO_MAIN        0x122U
#define CAN_ID_CAN_DIAG_STATUS    0x212U

#define ABS_MAIN_PERIOD_MS        10U
#define ABS_CAN_DIAG_PERIOD_MS    500U
#define CAN_TIMEOUT_CNT           1000000UL
#define UART_TIMEOUT_CNT          100000UL

#endif /* PROJECT_CFG_H */

