#include "mock.h"
#include "Can.h"
#include "Can_Cfg.h"
#include "BuzzerIf.h"
#include "Det.h"
#include "OsIf.h"
#include "I2c.h"
#include <string.h>
#include <stdarg.h>
MockFrame mock_frames[320];
unsigned mock_n, mock_reads, mock_buzzer_edges, mock_delay_calls, mock_max_delay, mock_cfg_writes;
uint8 mock_last_config, mock_last_accel_config;
boolean mock_buzzer;
uint32 mock_now;
static boolean sensor_error, tx_error, bad_whoami;
static Mpu6050If_AxesType axes;
void mock_reset(void) {
    mock_n=0;mock_reads=0;mock_buzzer_edges=0;mock_delay_calls=0;mock_max_delay=0;
    mock_now=0;mock_buzzer=FALSE;sensor_error=FALSE;tx_error=FALSE;bad_whoami=FALSE;
    mock_cfg_writes=0;mock_last_config=0;mock_last_accel_config=0;
    memset(mock_frames,0,sizeof(mock_frames));mock_axes(0,0,4096);
}
void mock_axes(sint16 x,sint16 y,sint16 z){axes.x=x;axes.y=y;axes.z=z;}
void mock_fail_sensor(boolean yes){sensor_error=yes;}
void mock_bad_whoami(boolean yes){bad_whoami=yes;}
void mock_fail_tx(boolean yes){tx_error=yes;}
void mock_advance(uint32 ms){mock_now+=ms;}
uint32 OsIf_GetTimeMs(void){return mock_now;}
void OsIf_DelayMs(uint32 ms){mock_delay_calls++;if(ms>mock_max_delay)mock_max_delay=ms; mock_now+=ms;}
Std_ReturnType I2c_ReadReg(uint8 addr,uint8 reg,uint8 *data) {
    if (addr!=MPU6050_I2C_ADDR || reg!=MPU6050_WHO_AM_I_REG || !data || sensor_error)return E_NOT_OK;
    *data=bad_whoami?0xFFU:MPU6050_WHO_AM_I_VALUE;return E_OK;
}
Std_ReturnType I2c_WriteReg(uint8 addr,uint8 reg,uint8 value) {
    if (addr!=MPU6050_I2C_ADDR || sensor_error)return E_NOT_OK;
    mock_cfg_writes++;
    if(reg==MPU6050_CONFIG)mock_last_config=value;
    if(reg==MPU6050_ACCEL_CONFIG)mock_last_accel_config=value;
    return E_OK;
}
Std_ReturnType I2c_ReadRegMulti(uint8 addr,uint8 reg,uint8 *buf,uint8 len) {
    mock_reads++;
    if (sensor_error || addr!=MPU6050_I2C_ADDR || reg!=MPU6050_ACCEL_XOUT_H || len!=6U || !buf)return E_NOT_OK;
    sint16 raw[3]={axes.x,axes.y,axes.z};
    for(unsigned i=0;i<3;i++){buf[2*i]=(uint8)(((uint16)raw[i]>>8)&0xFF);buf[2*i+1]=(uint8)(raw[i]&0xFF);}
    return E_OK;
}
Std_ReturnType BuzzerIf_Set(boolean on){if(mock_buzzer != on)mock_buzzer_edges++; mock_buzzer=on;return E_OK;}
void Det_Print(const char *s){(void)s;}
void Det_Printf(const char *fmt,...){(void)fmt;}
Std_ReturnType Det_ReportError(uint16 m,uint8 i,uint8 a,uint8 e){(void)m;(void)i;(void)a;(void)e;return E_OK;}
Std_ReturnType Det_ReportRuntimeError(uint16 m,uint8 i,uint8 a,uint8 e){(void)m;(void)i;(void)a;(void)e;return E_OK;}
Can_ReturnType Can_Write(Can_HwHandleType h,const Can_PduType *p){
    (void)h;
    if(!p || !p->sdu || p->length>8 || mock_n>=320)return CAN_NOT_OK;
    MockFrame *r=&mock_frames[mock_n++];r->id=p->id;r->dlc=p->length;r->when=mock_now;
    memcpy(r->data,p->sdu,p->length);r->ok=!tx_error;
    return tx_error?CAN_BUSY:CAN_OK;
}
Std_ReturnType Can_GetDebugStatus(uint8 c,Can_DebugStatusType *s){
    if(c!=CAN_MAIN_CONTROLLER || !s)return E_NOT_OK;
    memset(s,0,sizeof(*s));s->lec=2;s->tec=3;s->rec=4;s->errorState=CAN_ERRORSTATE_ACTIVE;s->controllerMode=CAN_CS_STARTED;
    return E_OK;
}
Std_ReturnType Can_SetControllerMode(uint8 c,Can_ControllerStateType mode){(void)c;(void)mode;return E_OK;}
Std_ReturnType Can_GetControllerMode(uint8 c,Can_ControllerStateType *m){(void)c;if(!m)return E_NOT_OK;*m=CAN_CS_STARTED;return E_OK;}
Std_ReturnType Can_GetControllerErrorState(uint8 c,Can_ErrorStateType *e){(void)c;if(!e)return E_NOT_OK;*e=CAN_ERRORSTATE_ACTIVE;return E_OK;}
void Can_MainFunction_BusOff(void){}
void Can_MainFunction_Mode(void){}
void Can_MainFunction_Write(void){}
void Can_MainFunction_Read(void){}
