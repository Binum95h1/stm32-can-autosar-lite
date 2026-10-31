#include "AirbagS.h"
#include "AirbagS_Types.h"
#include "AirbagS_Cfg.h"
#include "BuzzerIf.h"
#include "BareCan.h"
#include "Can.h"
#include "Can_Cfg.h"
#include "OsIf.h"
#include "Det.h"
#include "Mpu6050If.h"
#include "BareCan_Cfg.h"

static AirbagS_DeployStateType deploy_flag;
static uint32 last_print;
static uint32 last_can_diag_tx;
static uint32 last_tx_stats_print;
static boolean sensor_fault_active;
static uint32 sensor_fault_start;
static Mpu6050If_AxesType accel_raw;

static boolean AirbagS_IsCollisionDetected(const Mpu6050If_AxesType *a)
{
    sint32 x = Mpu6050If_RawToMg(a->x);
    sint32 y = Mpu6050If_RawToMg(a->y);
    sint32 z = Mpu6050If_RawToMg(a->z);
    sint64 total = ((sint64)x * x) + ((sint64)y * y) + ((sint64)z * z);
    return (total > (sint64)AIRBAGS_FORCE_THRESHOLD_SQ) ? TRUE : FALSE;
}

static void AirbagS_SendCanDiagnosticIfDue(void)
{
    uint32 now = OsIf_GetTimeMs();
    Can_DebugStatusType status;

    if ((now - last_can_diag_tx) < AIRBAGS_CAN_DIAG_PERIOD_MS) {
        return;
    }

    last_can_diag_tx = now;

    if (Can_GetDebugStatus(CAN_MAIN_CONTROLLER,&status) == E_OK) {
        (void)BareCan_SendCanDiagStatus(&status);
    }
}

static void AirbagS_PrintTxStatsIfDue(void)
{
    uint32 now = OsIf_GetTimeMs();

    if ((now - last_tx_stats_print) < 1000U) {
        return;
    }

    last_tx_stats_print = now;
    Det_Printf("[AIRBAG TX STAT] ID=0x%03lX TX_OK=%lu TX_FAIL=%lu SEQ=%u\r\n",
                              (unsigned long)CAN_ID_AIRBAG_TO_MAIN,
                              (unsigned long)BareCan_GetDeployTxOkCount(),
                              (unsigned long)BareCan_GetDeployTxFailCount(),
                              BareCan_GetDeploySeqCounter());
}

void AirbagS_Init(void)
{
    deploy_flag = FALSE;
    last_print = 0U;
    last_can_diag_tx = 0U;
    last_tx_stats_print = 0U;
    sensor_fault_active = FALSE;
    sensor_fault_start = 0U;
}

void AirbagS_MainFunction(void)
{
    if (deploy_flag == FALSE) {
        if (Mpu6050If_ReadAxes(&accel_raw) == E_OK) {
            sint32 x = Mpu6050If_RawToMg(accel_raw.x);
            sint32 y = Mpu6050If_RawToMg(accel_raw.y);
            sint32 z = Mpu6050If_RawToMg(accel_raw.z);

            if ((OsIf_GetTimeMs() - last_print) >= AIRBAGS_LOG_PERIOD_MS) {
                last_print = OsIf_GetTimeMs();
                Det_Printf("RAW X=%d Y=%d Z=%d | mg X=%ld Y=%ld Z=%ld\r\n",
                                          accel_raw.x, accel_raw.y, accel_raw.z,
                                          (long)x, (long)y, (long)z);
            }

            if (AirbagS_IsCollisionDetected(&accel_raw) == TRUE) {
                deploy_flag = TRUE;
                Det_Print("!!! COLLISION DETECTED -> BUZZER ON !!!\r\n");
            }
            sensor_fault_active = FALSE;
        } else {
            if (sensor_fault_active == FALSE) {
                sensor_fault_active = TRUE;
                sensor_fault_start = OsIf_GetTimeMs();
                Det_Print("MPU6050 READ FAIL\r\n");
            }
        }
    }

    /* Priority: latched collision buzzer > sensor-fault intermittent warning. */
    {
        uint32 now = OsIf_GetTimeMs();
        boolean fault_beep = (sensor_fault_active == TRUE) &&
            (((uint32)(now - sensor_fault_start) % AIRBAGS_SENSOR_FAULT_BUZZER_PERIOD_MS)
                < AIRBAGS_SENSOR_FAULT_BUZZER_ON_MS);
        (void)BuzzerIf_Set(((deploy_flag == TRUE) || (fault_beep == TRUE)) ? TRUE : FALSE);
    }

    if (BareCan_SendDeployFlag(deploy_flag) != E_OK) {
        Det_Print("CAN TX failed or mailbox full\r\n");
    }

    AirbagS_SendCanDiagnosticIfDue();
    AirbagS_PrintTxStatsIfDue();
}
