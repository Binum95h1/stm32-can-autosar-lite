#ifndef PROJECT_CFG_H
#define PROJECT_CFG_H

#define SYSCLK_HZ                 72000000UL
#define APB1_CLK_HZ               36000000UL

#define CAN_ID_AIRBAG_TO_MAIN     0x111U
#define CAN_ID_MAIN_TO_ABS        0x121U
#define CAN_ID_ABS_TO_MAIN        0x122U
#define CAN_ID_CAN_DIAG_STATUS    0x211U

#define MAIN_TASK_PERIOD_MS       10U
#define MAIN_ABS_TX_PERIOD_MS     10U
#define MAIN_LOG_PERIOD_MS        500U
#define MAIN_LCD_PERIOD_MS        250U
#define MAIN_CAN_DIAG_PERIOD_MS   100U

#define CAN_TIMEOUT_CNT           100000UL
#define UART_TIMEOUT_CNT          100000UL
#define ADC_TIMEOUT_CNT           1000000UL

#define LCD_ROW_LEN               16U
#define LCD_BUF_LEN               17U
#define LCD_BLANK_LINE            "                "
#define LCD_DANGER_LINE           "AIRBAG: DANGER "
#define LCD_SAFE_LINE             "AIRBAG: SAFE   "
#define LCD_BRAKE_LINE            "   Car Braked   "

#define BUTTON_PRESSED            1U
#define BUTTON_RELEASED           0U
#define MOVAVG_OLD_WEIGHT         3U
#define MOVAVG_DIVISOR            4U
#define DISPLAY_ZERO_THRESHOLD    2U

#endif /* PROJECT_CFG_H */
