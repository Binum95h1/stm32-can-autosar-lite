#include "MainNodeS.h"
#include "MainNodeS_Internal.h"
#include "Rte_Main.h"
#include "Project_Cfg.h"
#include <stdio.h>
#include <string.h>

void MainNodeS_ReadLocalInputs(void)
{
    adc_val = Rte_Call_MainS_Adc_ReadThrottleRaw();
    sw1 = (Rte_Call_MainS_Dio_ReadBrakeSwitch() != 0U) ? BUTTON_PRESSED : BUTTON_RELEASED;
    sw2 = (Rte_Call_MainS_Dio_ReadDirectionSwitch() != 0U) ? BUTTON_PRESSED : BUTTON_RELEASED;
    dir_sw = sw2;
}

void MainNodeS_SendCommandToAbs(void)
{
    uint16 adc_to_send;
    uint8  brake_to_send;
    uint8  dir_to_send;
    Std_ReturnType can_ok;
    uint8 seq_to_send;
    uint8 raw0;
    uint8 raw1;

    MainNodeS_ReadLocalInputs();

    adc_to_send = (uint16)(adc_val & 0x0FFFU);
    brake_to_send = sw1;
    dir_to_send = sw2;

    if (emergency_stop != 0U) {
        adc_to_send = 0U;
        brake_to_send = 1U;
    }

    seq_to_send = main_to_abs_seq;
    raw0 = (uint8)(adc_to_send & 0xFFU);
    raw1 = (uint8)((adc_to_send >> 8U) & 0x0FU);

    can_ok = Rte_Call_MainS_Com_SendMainCommand(adc_to_send, brake_to_send, dir_to_send, seq_to_send);

    if (can_ok == E_OK) {
        if (emergency_stop != 0U) {
            Rte_Call_MainS_Det_Logf("[MAIN TX ABS] ID=0x121 | EMERGENCY STOP | SEQ=%u | TX_OK=%lu | RAW=%02X %02X %02X %02X %02X\r\n",
                   seq_to_send, (unsigned long)(main_to_abs_tx_count + 1UL),
                   raw0, raw1, brake_to_send, dir_to_send, seq_to_send);
        } else {
            Rte_Call_MainS_Det_Logf("[MAIN TX ABS] ID=0x121 | ADC=%lu | BRAKE=%u | DIR=%u | SEQ=%u | TX_OK=%lu | RAW=%02X %02X %02X %02X %02X\r\n",
                   (unsigned long)adc_to_send, brake_to_send, dir_to_send, seq_to_send,
                   (unsigned long)(main_to_abs_tx_count + 1UL),
                   raw0, raw1, brake_to_send, dir_to_send, seq_to_send);
        }
        main_to_abs_tx_count++;
        main_to_abs_seq++;
    } else {
        /* Do not spam UART when bxCAN mailboxes are locked by retransmission during bus-fault tests. */
    }
}

void MainNodeS_PrintAbsRx(void)
{
    Rte_Call_MainS_Det_Logf("[MAIN RX ABS] ID=0x122 | V1=%u cm/s | V2=%u cm/s | STATE=%u | RX=%lu | LOST=%lu | SEQ=%u\r\n",
           abs1, abs2, abs3,
           (unsigned long)main_abs_rx_count,
           (unsigned long)main_abs_lost_count,
           main_abs_last_seq);
}

void MainNodeS_PrintAirbagRx(void)
{
    Rte_Call_MainS_Det_Logf("[MAIN RX AIRBAG] ID=0x111 | DEPLOY=%u | SEQ=%u | RX=%lu | LOST=%lu\r\n",
           airbag1,
           airbag2,
           (unsigned long)main_airbag_rx_count,
           (unsigned long)main_airbag_lost_count);
}

static void MainNodeS_PollComRx(void)
{
    ComIf_AbsFeedbackType absFeedback;
    ComIf_AirbagStatusType airbagStatus;

    if (Rte_Read_MainS_AbsFeedback(&absFeedback) == E_OK) {
        abs1 = absFeedback.wheel1Speed;
        abs2 = absFeedback.wheel2Speed;
        abs3 = absFeedback.motorState;

        main_abs_rx_count++;
        if (absFeedback.dlc >= 4U) {
            uint8 seq = absFeedback.sequence;
            if (main_abs_seq_valid == 0U) {
                main_abs_last_seq = seq;
                main_abs_seq_valid = 1U;
            } else {
                uint8 delta = (uint8)(seq - main_abs_last_seq);
                if ((delta > 0U) && (delta < 128U)) {
                    main_abs_lost_count += (uint32)(delta - 1U);
                    main_abs_last_seq = seq;
                }
                /* delta == 0: duplicate. delta >= 128: stale/out-of-order. */
            }
        }

        abs_new_data = 1U;
    }

    if (Rte_Read_MainS_AirbagStatus(&airbagStatus) == E_OK) {
        airbag1 = airbagStatus.deploy;
        airbag2 = airbagStatus.sequence;
        airbag3 = airbagStatus.reserved;

        main_airbag_rx_count++;
        if (airbagStatus.dlc >= 2U) {
            uint8 seq = airbagStatus.sequence;
            if (main_airbag_seq_valid == 0U) {
                main_airbag_last_seq = seq;
                main_airbag_seq_valid = 1U;
            } else {
                uint8 delta = (uint8)(seq - main_airbag_last_seq);
                if ((delta > 0U) && (delta < 128U)) {
                    main_airbag_lost_count += (uint32)(delta - 1U);
                    main_airbag_last_seq = seq;
                }
                /* delta == 0: duplicate. delta >= 128: stale/out-of-order. */
            }
        }

        emergency_stop = (airbag1 > 0U) ? 1U : 0U;
        airbag_new_data = 1U;
    }
}

void MainNodeS_UpdateDisplaySpeed(void)
{
    uint16 avg_temp;
    uint16 filt_temp;

    speed_left_cmps = abs1;
    speed_right_cmps = abs2;

    avg_temp = (uint16)speed_left_cmps + (uint16)speed_right_cmps;
    speed_avg_cmps = (uint8)(avg_temp / 2U);

    if (speed_avg_cmps < DISPLAY_ZERO_THRESHOLD) {
        speed_avg_cmps = 0U;
    }

    filt_temp = ((uint16)speed_display * MOVAVG_OLD_WEIGHT) + (uint16)speed_avg_cmps;
    speed_display = (uint8)(filt_temp / MOVAVG_DIVISOR);

    if (speed_display < DISPLAY_ZERO_THRESHOLD) {
        speed_display = 0U;
    }
}

void MainNodeS_UpdateLcd(void)
{
    int ret0;
    int ret1;
    const char *dir_text;

    MainNodeS_UpdateDisplaySpeed();

    if (emergency_stop != 0U) {
        if (blink_state != 0U) {
            ret0 = snprintf(lcd_row0, sizeof(lcd_row0), "%s", LCD_DANGER_LINE);
        } else {
            ret0 = snprintf(lcd_row0, sizeof(lcd_row0), "%s", LCD_BLANK_LINE);
        }
        blink_state = (blink_state == 0U) ? 1U : 0U;
    } else {
        ret0 = snprintf(lcd_row0, sizeof(lcd_row0), "%s", LCD_SAFE_LINE);
        blink_state = 1U;
    }

    if (ret0 < 0) {
        (void)strncpy(lcd_row0, LCD_BLANK_LINE, sizeof(lcd_row0));
        lcd_row0[LCD_BUF_LEN - 1U] = '\0';
    }

    Rte_Call_MainS_Lcd_SetCursor(0U, 0U);
    Rte_Call_MainS_Lcd_SendString(LCD_BLANK_LINE);
    Rte_Call_MainS_Lcd_SetCursor(0U, 0U);
    Rte_Call_MainS_Lcd_SendString(lcd_row0);

    if (emergency_stop != 0U) {
        ret1 = snprintf(lcd_row1, sizeof(lcd_row1), "%s", LCD_BRAKE_LINE);
    } else {
        dir_text = (dir_sw == 0U) ? "FWD" : "REV";
        ret1 = snprintf(lcd_row1, sizeof(lcd_row1), "D:%-3s V:%3u cm/s", dir_text, speed_display);
    }

    if (ret1 < 0) {
        (void)strncpy(lcd_row1, LCD_BLANK_LINE, sizeof(lcd_row1));
        lcd_row1[LCD_BUF_LEN - 1U] = '\0';
    }

    Rte_Call_MainS_Lcd_SetCursor(1U, 0U);
    Rte_Call_MainS_Lcd_SendString(LCD_BLANK_LINE);
    Rte_Call_MainS_Lcd_SetCursor(1U, 0U);
    Rte_Call_MainS_Lcd_SendString(lcd_row1);
}

static const char *MainNodeS_CanLecToString(uint8 lec)
{
    switch (lec) {
        case 0U: return "NO ERROR";
        case 1U: return "STUFF ERROR";
        case 2U: return "FORM ERROR";
        case 3U: return "ACK ERROR";
        case 4U: return "BIT ERROR - RECESSIVE";
        case 5U: return "BIT ERROR - DOMINANT";
        case 6U: return "CRC ERROR";
        case 7U: return "NO CHANGE";
        default: return "UNKNOWN";
    }
}

static const char *MainNodeS_CanStateToString(Can_ErrorStateType state)
{
    switch (state) {
        case CAN_ERRORSTATE_ACTIVE: return "ACTIVE";
        case CAN_ERRORSTATE_PASSIVE: return "PASSIVE";
        case CAN_ERRORSTATE_BUSOFF: return "BUS-OFF";
        default: return "UNKNOWN";
    }
}

static const char *MainNodeS_CanModeToString(Can_ControllerStateType mode)
{
    switch (mode) {
        case CAN_CS_STOPPED: return "STOPPED";
        case CAN_CS_STARTED: return "STARTED";
        case CAN_CS_SLEEP: return "SLEEP";
        case CAN_CS_UNINIT: return "UNINIT";
        default: return "UNKNOWN";
    }
}

static void MainNodeS_SendCanDiagnosticIfDue(void)
{
    static uint32 last_can_diag = 0U;
    uint32 now;
    Can_DebugStatusType status;

    now = Rte_Call_MainS_OsIf_GetTimeMs();

    if ((now - last_can_diag) < MAIN_CAN_DIAG_PERIOD_MS) {
        return;
    }

    last_can_diag = now;

    if (Rte_Call_MainS_CanIf_GetDebugStatus(&status) == E_OK) {
        (void)Rte_Call_MainS_Com_SendCanDiagStatus(&status);

        Rte_Call_MainS_Det_Logf("[CAN DIAG] NODE=MAIN | ID=0x211 | LEC=%u (%s) | TEC=%u | REC=%u | EWGF=%u | EPVF=%u | BOFF=%u | STATE=%s | MODE=%s\r\n",
                   status.lec,
                   MainNodeS_CanLecToString(status.lec),
                   status.tec,
                   status.rec,
                   status.ewgf,
                   status.epvf,
                   status.boff,
                   MainNodeS_CanStateToString(status.errorState),
                   MainNodeS_CanModeToString(status.controllerMode));
    } else {
        Rte_Call_MainS_Det_Log("[CAN DIAG] NODE=MAIN | GetDebugStatus FAIL\r\n");
    }
}

void MainNodeS_Init(void)
{
    Rte_Call_MainS_Lcd_Init();
    Rte_Call_MainS_Lcd_Clear();
    MainNodeS_UpdateLcd();
}

void MainNodeS_MainFunction(void)
{
    static uint32 last_lcd = 0U;
    static uint32 last_log = 0U;
    static uint32 last_abs_tx = 0U;
    uint32 now;

    now = Rte_Call_MainS_OsIf_GetTimeMs();

    MainNodeS_PollComRx();

    if ((now - last_abs_tx) >= MAIN_ABS_TX_PERIOD_MS) {
        last_abs_tx = now;
        MainNodeS_SendCommandToAbs();
    }

    MainNodeS_SendCanDiagnosticIfDue();

    if ((now - last_log) >= MAIN_LOG_PERIOD_MS) {
        last_log = now;

        if (abs_new_data != 0U) {
            abs_new_data = 0U;
            MainNodeS_PrintAbsRx();
        }

        if (airbag_new_data != 0U) {
            airbag_new_data = 0U;
            MainNodeS_PrintAirbagRx();
        }
    }

    if ((now - last_lcd) >= MAIN_LCD_PERIOD_MS) {
        last_lcd = now;
        MainNodeS_UpdateLcd();
    }
}
