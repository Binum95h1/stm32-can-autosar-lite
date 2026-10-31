#include "Mpu6050If.h"
#include "I2c.h"
#include "OsIf.h"
static uint8 who_am_i;
static boolean sensor_configured;
uint8 Mpu6050If_GetWhoAmI(void) { return who_am_i; }
sint32 Mpu6050If_RawToMg(sint16 raw)
{
    return ((sint32)raw * MPU6050_MG_NUMERATOR) / MPU6050_MG_DENOMINATOR;
}
Std_ReturnType Mpu6050If_Init(void)
{
    uint8 id = 0U;
    sensor_configured = FALSE;
    who_am_i = 0U;
    if (I2c_ReadReg(MPU6050_I2C_ADDR, MPU6050_WHO_AM_I_REG, &id) != E_OK) return E_NOT_OK;
    who_am_i = id;
    if (id != MPU6050_WHO_AM_I_VALUE) return E_NOT_OK;
    if (I2c_WriteReg(MPU6050_I2C_ADDR, MPU6050_PWR_MGMT_1, MPU6050_PWR1_WAKE) != E_OK) return E_NOT_OK;
    OsIf_DelayMs(10U); /* Only once during initialization, not the 10-ms task. */
    if (I2c_WriteReg(MPU6050_I2C_ADDR, MPU6050_CONFIG, MPU6050_DLPF_CFG3) != E_OK) return E_NOT_OK;
    if (I2c_WriteReg(MPU6050_I2C_ADDR, MPU6050_SMPLRT_DIV, MPU6050_SMPLRT_100HZ) != E_OK) return E_NOT_OK;
    if (I2c_WriteReg(MPU6050_I2C_ADDR, MPU6050_ACCEL_CONFIG, MPU6050_ACCEL_FS_8G) != E_OK) return E_NOT_OK;
    sensor_configured = TRUE;
    return E_OK;
}
Std_ReturnType Mpu6050If_ReadAxes(Mpu6050If_AxesType *axes)
{
    uint8 b[6];
    if (axes == NULL_PTR || sensor_configured == FALSE) return E_NOT_OK;
    if (I2c_ReadRegMulti(MPU6050_I2C_ADDR, MPU6050_ACCEL_XOUT_H, b, 6U) != E_OK) return E_NOT_OK;
    /* MPU6050 sends MSB first, unlike the old LIS331HH interface. */
    axes->x = (sint16)((((uint16)b[0]) << 8) | b[1]);
    axes->y = (sint16)((((uint16)b[2]) << 8) | b[3]);
    axes->z = (sint16)((((uint16)b[4]) << 8) | b[5]);
    return E_OK;
}
