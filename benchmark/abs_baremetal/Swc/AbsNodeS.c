#include "AbsNodeS.h"
#include "BareCan.h"
#include "MotorIf.h"
#include "OsIf.h"
#include "Det.h"
#include "Can.h"
#include "Can_Cfg.h"
#include "Project_Cfg.h"




#include <stdint.h>

/* ============================================================
 *  Wheel / Encoder configuration
 * ============================================================ */

#define WHEEL_DIAMETER_MM          65.0f
#define WHEEL_CIRCUM_MM            (3.1415926f * WHEEL_DIAMETER_MM)

#define ENCODER_PPR_MOTOR          11.0f
#define GEAR_RATIO                 21.3f
#define ENCODER_MULTIPLIER         4.0f

#define COUNTS_PER_WHEEL_REV       (ENCODER_PPR_MOTOR * GEAR_RATIO * ENCODER_MULTIPLIER)

/* ============================================================
 *  Timing configuration
 * ============================================================ */

#define SPEED_SAMPLE_TIME_MS        100U
#define SPEED_SAMPLE_TIME_S         0.1f

#define CAN_TX_PERIOD_MS            10U
#define CAN_DIAG_PERIOD_MS          500U
#define CAN_STAT_PERIOD_MS          1000U

#define CAN_RX_TIMEOUT_MS           300U
#define MOTOR_MONITOR_PERIOD_MS     100U

/* ============================================================
 *  Motor / PWM configuration
 * ============================================================ */

#define PWM_DEAD_ADC                10U
#define PWM_MIN_PERCENT             50U
#define PWM_MAX_PERCENT             80U
#define ADC_FULL_SCALE              4095U

#define MOTOR_PWM_RUN_THRESHOLD     3000U
#define WHEEL_ZERO_SPEED_CMPS       2U
#define MOTOR_NO_RESPONSE_LIMIT     5U

/* ============================================================
 *  Motor state
 * ============================================================ */

#define MOTOR_STATE_STOP            0U
#define MOTOR_STATE_FORWARD         1U
#define MOTOR_STATE_REVERSE         2U
#define MOTOR_STATE_BRAKE           3U
#define MOTOR_STATE_FAULT           4U

/* ============================================================
 *  Diagnostic Trouble Code
 * ============================================================ */

#define DTC_NONE                    0x00000000UL
#define DTC_CAN_TIMEOUT             0x00000001UL
#define DTC_MOTOR_NO_RESPONSE       0x00000002UL

/* ============================================================
 *  Static variables
 * ============================================================ */

static uint8_t main_cmd1;
static uint8_t main_cmd2;
static uint8_t main_sw1;
static uint8_t main_sw2;

static uint8_t abs_rx_new_data;
static uint8_t abs_to_main_cnt;

static uint8_t main_rx_seq;
static uint8_t main_rx_last_seq;
static uint8_t main_rx_seq_valid;

static uint32_t abs_rx_main_count;
static uint32_t abs_rx_main_lost;
static uint32_t abs_tx_main_ok;
static uint32_t abs_tx_main_fail;

static uint16_t adc_raw_from_main;
static uint16_t pwm_motor1;
static uint16_t pwm_motor2;

static uint8_t motor_state;
static uint8_t wheel1_speed;
static uint8_t wheel2_speed;
static uint8_t motor_no_response_cnt;

static int32_t enc1_prev;
static int32_t enc2_prev;

static uint32_t speed_last_tick;
static uint32_t can_tx_last_tick;
static uint32_t can_diag_last_tick;
static uint32_t can_stat_last_tick;
static uint32_t abs_rx_last_tick;
static uint32_t motor_monitor_last_tick;

static uint32_t abs_dtc_flags;

/* ============================================================
 *  Local functions
 * ============================================================ */

static uint16_t Map_ADC_To_PWM(uint16_t adc)
{
    uint32_t arr;
    uint32_t min;
    uint32_t max;
    uint32_t ccr;

    arr = MotorIf_GetPwmPeriod();
    min = (arr * PWM_MIN_PERCENT) / 100U;
    max = (arr * PWM_MAX_PERCENT) / 100U;

    if (adc <= PWM_DEAD_ADC)
    {
        return 0U;
    }

    ccr = min +
          ((uint32_t)(adc - PWM_DEAD_ADC) * (max - min)) /
          (ADC_FULL_SCALE - PWM_DEAD_ADC);

    if (ccr > max)
    {
        ccr = max;
    }

    return (uint16_t)ccr;
}

static void Motor_Stop(void)
{
    MotorIf_SetPwm(0U, 0U);
    MotorIf_SetDirection1(0U, 0U);
    MotorIf_SetDirection2(0U, 0U);

    motor_state = MOTOR_STATE_STOP;
}

static void Motor_Forward(uint16_t pwm)
{
    MotorIf_SetDirection1(1U, 0U);
    MotorIf_SetDirection2(1U, 0U);
    MotorIf_SetPwm(pwm, pwm);

    motor_state = MOTOR_STATE_FORWARD;
}

static void Motor_Reverse(uint16_t pwm){
    MotorIf_SetDirection1(0U, 1U);
    MotorIf_SetDirection2(0U, 1U);
    MotorIf_SetPwm(pwm, pwm);

    motor_state = MOTOR_STATE_REVERSE;
}

static void ABS_EnterSafeState(uint32_t dtc){
    abs_dtc_flags |= dtc;

    MotorIf_SetPwm(0U, 0U);
    MotorIf_SetDirection1(0U, 0U);
    MotorIf_SetDirection2(0U, 0U);

    pwm_motor1 = 0U;
    pwm_motor2 = 0U;

    motor_state = MOTOR_STATE_FAULT;
}

/* ============================================================
 *  Debug log functions
 * ============================================================ */

static void Print_MAIN_RX(void){
    Det_Printf(
        "[ABS RX MAIN] ID=0x121 | ADC=%u | BRAKE=%u | DIR=%u | "
        "SEQ=%u | RX=%lu | LOST=%lu\r\n",
        adc_raw_from_main,
        main_sw1,
        main_sw2,
        main_rx_seq,
        abs_rx_main_count,
        abs_rx_main_lost
    );
}

static void Print_ABS_TX(const uint8_t *data){
    Det_Printf(
        "[ABS TX MAIN] ID=0x122 | W1=%u cm/s | W2=%u cm/s | "
        "STATE=%u | SEQ=%u | TX_OK=%lu | TX_FAIL=%lu\r\n",
        data[0],
        data[1],
        data[2],
        data[3],
        abs_tx_main_ok,
        abs_tx_main_fail
    );
}

static void Print_ABS_STAT(void){
    Det_Printf(
        "[ABS STAT] RX_MAIN=%lu | LOST_MAIN=%lu | "
        "TX_MAIN_OK=%lu | TX_MAIN_FAIL=%lu\r\n",
        abs_rx_main_count,
        abs_rx_main_lost,
        abs_tx_main_ok,
        abs_tx_main_fail
    );
}

/* ============================================================
 *  Motor control
 * ============================================================ */

static void Motor_Control_From_MAIN(void)
{
    uint16_t pwm;

    pwm = Map_ADC_To_PWM(adc_raw_from_main);

    pwm_motor1 = pwm;
    pwm_motor2 = pwm;

    Det_Printf(
        "[ABS MOTOR] ADC=%u | PWM=%u | BRAKE=%u | DIR=%u\r\n",
        adc_raw_from_main,
        pwm,
        main_sw1,
        main_sw2
    );

    if (main_sw1 == 1U)
    {
        Motor_Stop();
        motor_state = MOTOR_STATE_BRAKE;
    }
    else if (adc_raw_from_main <= PWM_DEAD_ADC)
    {
        Motor_Stop();
    }
    else if (main_sw2 == 0U)
    {
        Motor_Forward(pwm);
    }
    else
    {
        Motor_Reverse(pwm);
    }
}

/* ============================================================
 *  Encoder speed calculation
 * ============================================================ */

static void Encoder_Update(void)
{
    uint32_t now;
    int32_t enc1_now;
    int32_t enc2_now;
    int32_t delta1;
    int32_t delta2;
    float speed1_mps;
    float speed2_mps;
    float speed1_cmps;
    float speed2_cmps;

    now = OsIf_GetTimeMs();

    if ((now - speed_last_tick) < SPEED_SAMPLE_TIME_MS)
    {
        return;
    }

    speed_last_tick = now;

    enc1_now = MotorIf_GetEncoderCount1();
    enc2_now = MotorIf_GetEncoderCount2();

    delta1 = enc1_now - enc1_prev;
    delta2 = enc2_now - enc2_prev;

    if (delta1 > 32767L)
    {
        delta1 -= 65536L;
    }
    else if (delta1 < -32768L)
    {
        delta1 += 65536L;
    }

    if (delta2 > 32767L)
    {
        delta2 -= 65536L;
    }
    else if (delta2 < -32768L)
    {
        delta2 += 65536L;
    }

    enc1_prev = enc1_now;
    enc2_prev = enc2_now;

    if (delta1 < 0L)
    {
        delta1 = -delta1;
    }

    if (delta2 < 0L)
    {
        delta2 = -delta2;
    }

    speed1_mps = (((float)delta1 / COUNTS_PER_WHEEL_REV) *
                   WHEEL_CIRCUM_MM / 1000.0f) /
                   SPEED_SAMPLE_TIME_S;

    speed2_mps = (((float)delta2 / COUNTS_PER_WHEEL_REV) *
                   WHEEL_CIRCUM_MM / 1000.0f) /
                   SPEED_SAMPLE_TIME_S;

    speed1_cmps = speed1_mps * 100.0f;
    speed2_cmps = speed2_mps * 100.0f;

    if (speed1_cmps > 255.0f)
    {
        speed1_cmps = 255.0f;
    }

    if (speed2_cmps > 255.0f)
    {
        speed2_cmps = 255.0f;
    }

    wheel1_speed = (uint8_t)(speed1_cmps + 0.5f);
    wheel2_speed = (uint8_t)(speed2_cmps + 0.5f);

    Det_Printf(
        "[ENC] d1=%ld d2=%ld | V1=%u cm/s | V2=%u cm/s\r\n",
        delta1,
        delta2,
        wheel1_speed,
        wheel2_speed
    );
}

/* ============================================================
 *  Safety monitor
 * ============================================================ */

static void ABS_SafetyMonitor(void)
{
    uint32_t now;

    now = OsIf_GetTimeMs();

    if ((now - abs_rx_last_tick) >= CAN_RX_TIMEOUT_MS)
    {
        ABS_EnterSafeState(DTC_CAN_TIMEOUT);
    }

    if ((now - motor_monitor_last_tick) >= MOTOR_MONITOR_PERIOD_MS)
    {
        motor_monitor_last_tick = now;

        if (((motor_state == MOTOR_STATE_FORWARD) ||
             (motor_state == MOTOR_STATE_REVERSE)) &&
            ((pwm_motor1 > MOTOR_PWM_RUN_THRESHOLD) ||
             (pwm_motor2 > MOTOR_PWM_RUN_THRESHOLD)))
        {
            if ((wheel1_speed <= WHEEL_ZERO_SPEED_CMPS) &&
                (wheel2_speed <= WHEEL_ZERO_SPEED_CMPS))
            {
                if (motor_no_response_cnt < 255U)
                {
                    motor_no_response_cnt++;
                }

                if (motor_no_response_cnt >= MOTOR_NO_RESPONSE_LIMIT)
                {
                    ABS_EnterSafeState(DTC_MOTOR_NO_RESPONSE);
                }
            }
            else
            {
                motor_no_response_cnt = 0U;
            }
        }
        else
        {
            motor_no_response_cnt = 0U;
        }
    }
}

/* ============================================================
 *  CAN transmit functions
 * ============================================================ */

static void Send_Data_To_MAIN(void)
{
    uint8_t data[8];

    data[0] = wheel1_speed;
    data[1] = wheel2_speed;
    data[2] = motor_state;
    data[3] = abs_to_main_cnt;
    data[4] = 0U;
    data[5] = 0U;
    data[6] = 0U;
    data[7] = 0U;

    if (BareCan_SendAbsFeedback(data[0],
                                          data[1],
                                          data[2],
                                          data[3]) == E_OK)
    {
        abs_tx_main_ok++;

        Print_ABS_TX(data);

        abs_to_main_cnt++;
    }
    else
    {
        abs_tx_main_fail++;
    }
}

static void Send_CanDiag(void)
{
    Can_DebugStatusType status;

    if (Can_GetDebugStatus(CAN_MAIN_CONTROLLER, &status) == E_OK)
    {
        (void)BareCan_SendCanDiagStatus(&status);

        if ((status.lec != 0U) ||
            (status.ewgf != 0U) ||
            (status.epvf != 0U) ||
            (status.boff != 0U))
        {
            Det_Printf(
                "[CAN DIAG] NODE=ABS | ID=0x212 | "
                "LEC=%u | TEC=%u | REC=%u | "
                "EWGF=%u | EPVF=%u | BOFF=%u\r\n",
                status.lec,
                status.tec,
                status.rec,
                status.ewgf,
                status.epvf,
                status.boff
            );
        }
    }
}

/* ============================================================
 *  ABS initialization
 * ============================================================ */

void AbsNodeS_Init(void)
{
    Det_Printf(
        "ABS NODE SAN SANG | RX:0x%03X | TX:0x%03X | DIAG:0x%03X\r\n"
        "=============================\r\n\r\n",
        CAN_ID_MAIN_TO_ABS,
        CAN_ID_ABS_TO_MAIN,
        CAN_ID_CAN_DIAG_STATUS
    );

    Motor_Stop();

    abs_rx_last_tick = OsIf_GetTimeMs();
    motor_monitor_last_tick = OsIf_GetTimeMs();
}

/* ============================================================
 *  CAN receive processing
 * ============================================================ */

static void AbsNodeS_ProcessMainCommand(const BareCan_MainCommandType *cmd)
{
    uint8_t expected_seq;

    if (cmd == NULL_PTR)
    {
        return;
    }

    main_cmd1 = (uint8_t)(cmd->adcRaw & 0xFFU);
    main_cmd2 = (uint8_t)((cmd->adcRaw >> 8U) & 0x0FU);

    main_sw1 = cmd->brake;
    main_sw2 = cmd->direction;

    main_rx_seq = cmd->sequence;

    if (cmd->dlc >= 5U)
    {
        if (main_rx_seq_valid != 0U)
        {
            expected_seq = (uint8_t)(main_rx_last_seq + 1U);

            if (main_rx_seq != expected_seq)
            {
                abs_rx_main_lost += (uint8_t)(main_rx_seq - expected_seq);
            }
        }
        else
        {
            if (main_rx_seq > 0U)
            {
                abs_rx_main_lost += main_rx_seq;
            }

            main_rx_seq_valid = 1U;
        }

        main_rx_last_seq = main_rx_seq;
    }

    abs_rx_main_count++;

    adc_raw_from_main = cmd->adcRaw;
    abs_rx_last_tick = OsIf_GetTimeMs();
    abs_rx_new_data = 1U;

    abs_dtc_flags &= ~DTC_CAN_TIMEOUT;
}

void AbsNodeS_CanRxIndication(uint32_t id, const uint8_t *data, uint8_t dlc)
{
    BareCan_MainCommandType cmd;

    if ((id == CAN_ID_MAIN_TO_ABS) && (dlc >= 4U))
    {
        cmd.adcRaw = (uint16_t)((uint16_t)data[0] |
                                (((uint16_t)data[1] & 0x0FU) << 8U));

        cmd.brake = data[2];
        cmd.direction = data[3];
        cmd.sequence = (dlc >= 5U) ? data[4] : 0U;
        cmd.dlc = dlc;

        AbsNodeS_ProcessMainCommand(&cmd);
    }
    else
    {
        Det_Printf(
            "[ABS RX KHAC] ID=0x%03lX DLC=%u\r\n",
            id,
            dlc
        );
    }
}

/* ============================================================
 *  ABS cyclic task
 * ============================================================ */

void AbsNodeS_MainFunction(void)
{
    BareCan_MainCommandType cmd;
    uint32_t now;

    if (BareCan_ReadMainCommand(&cmd) == E_OK)
    {
        AbsNodeS_ProcessMainCommand(&cmd);
    }

    if (abs_rx_new_data != 0U)
    {
        abs_rx_new_data = 0U;

        Print_MAIN_RX();

        if (abs_dtc_flags == DTC_NONE)
        {
            Motor_Control_From_MAIN();
        }
        else
        {
            ABS_EnterSafeState(abs_dtc_flags);
        }
    }

    Encoder_Update();
    ABS_SafetyMonitor();

    now = OsIf_GetTimeMs();

    if ((now - can_tx_last_tick) >= CAN_TX_PERIOD_MS)
    {
        can_tx_last_tick = now;
        Send_Data_To_MAIN();
    }

    if ((now - can_diag_last_tick) >= CAN_DIAG_PERIOD_MS)
    {
        can_diag_last_tick = now;
        Send_CanDiag();
    }

    if ((now - can_stat_last_tick) >= CAN_STAT_PERIOD_MS)
    {
        can_stat_last_tick = now;
        Print_ABS_STAT();
    }

}


