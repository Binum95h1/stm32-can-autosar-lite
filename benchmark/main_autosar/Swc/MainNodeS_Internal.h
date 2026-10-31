#ifndef MAINNODES_INTERNAL_H
#define MAINNODES_INTERNAL_H

#include "Std_Types.h"
#include "Project_Cfg.h"

extern uint16 adc_val;
extern uint8 sw1;
extern uint8 sw2;
extern uint8 dir_sw;
extern uint8 abs1;
extern uint8 abs2;
extern uint8 abs3;
extern uint8 airbag1;
extern uint8 airbag2;
extern uint8 airbag3;
extern volatile uint8 abs_new_data;
extern volatile uint8 airbag_new_data;
extern volatile uint8 emergency_stop;
extern uint32 main_to_abs_tx_count;
extern uint8 main_to_abs_seq;
extern uint32 main_abs_rx_count;
extern uint32 main_abs_lost_count;
extern uint8 main_abs_last_seq;
extern uint8 main_abs_seq_valid;
extern uint32 main_airbag_rx_count;
extern uint32 main_airbag_lost_count;
extern uint8 main_airbag_last_seq;
extern uint8 main_airbag_seq_valid;
extern char lcd_row0[LCD_BUF_LEN];
extern char lcd_row1[LCD_BUF_LEN];
extern uint8 blink_state;
extern uint8 speed_left_cmps;
extern uint8 speed_right_cmps;
extern uint8 speed_avg_cmps;
extern uint8 speed_display;

#endif /* MAINNODES_INTERNAL_H */
