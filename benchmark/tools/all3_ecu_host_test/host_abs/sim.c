#include "sim.h"
#include "Pwm_Encoder.h"
#include "Dio.h"
#include "MotorIf.h"
#include "OsIf.h"
#include "Det.h"
#include "Can.h"
#include "SchM.h"
#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include <math.h>
SimState sim;
void sim_reset(void){
    memset(&sim,0,sizeof(sim));
    sim.diag.controllerMode=CAN_CS_STARTED;
    sim.diag.errorState=CAN_ERRORSTATE_ACTIVE;
}
void sim_advance_10ms(void){
    for(int i=0;i<2;i++){
        int dir=(sim.dir[i][0] && !sim.dir[i][1])?1:((!sim.dir[i][0] && sim.dir[i][1])?-1:0);
        float desired=sim.motor_stalled?0.0f:(float)dir*(float)sim.pwm[i]/10000.0f*1.2f;
        sim.pulse_rate[i]=sim.pulse_rate[i]*0.60f+desired*0.40f;
        sim.encoder_fraction[i]+=sim.pulse_rate[i]*10.0f;
        int whole=(int)truncf(sim.encoder_fraction[i]);
        sim.encoder_fraction[i]-=(float)whole;
        sim.encoder[i]=(uint16_t)((int)sim.encoder[i]+whole);
    }
    sim.now_ms+=10;
}
void PwmEncoder_Init(void){}
void Pwm_SetMotor(uint16_t a,uint16_t b){sim.pwm[0]=a;sim.pwm[1]=b;}
uint16_t Pwm_GetPeriod(void){return 10000;}
uint16_t Encoder_GetCount1(void){return sim.encoder[0];}
uint16_t Encoder_GetCount2(void){return sim.encoder[1];}
void Dio_Init(void){}
void Dio_WriteMotor1(uint8_t a,uint8_t b){sim.dir[0][0]=a;sim.dir[0][1]=b;}
void Dio_WriteMotor2(uint8_t a,uint8_t b){sim.dir[1][0]=a;sim.dir[1][1]=b;}
uint32 OsIf_GetTimeMs(void){return sim.now_ms;}
void OsIf_DelayMs(uint32 ms){sim.now_ms+=ms;}
void Det_Init(void){}
void Det_Start(void){}
void Det_Print(const char *s){
    if(!s)return;
    size_t n=strlen(s);
    size_t available=(SIM_LOG_CAP-1)-(size_t)sim.log_length;
    if(n>available)n=available;
    if(n){memcpy(sim.log+sim.log_length,s,n);sim.log_length+=(uint32_t)n;sim.log[sim.log_length]='\0';}
}
void Det_Printf(const char *fmt,...){
    char temp[512];va_list ap;va_start(ap,fmt);
    vsnprintf(temp,sizeof(temp),fmt,ap);va_end(ap);
    Det_Print(temp);
}
Std_ReturnType Det_ReportError(uint16 m,uint8 i,uint8 a,uint8 e){(void)m;(void)i;(void)a;(void)e;return E_OK;}
Std_ReturnType Det_ReportRuntimeError(uint16 m,uint8 i,uint8 a,uint8 e){(void)m;(void)i;(void)a;(void)e;return E_OK;}
void Can_Init(const Can_ConfigType *cfg){(void)cfg;}
void Can_DeInit(void){}
Std_ReturnType Can_SetControllerMode(uint8 ctl,Can_ControllerStateType state){(void)ctl;sim.diag.controllerMode=state;return E_OK;}
Std_ReturnType Can_GetControllerMode(uint8 ctl,Can_ControllerStateType *st){(void)ctl;if(!st)return E_NOT_OK;*st=sim.diag.controllerMode;return E_OK;}
Std_ReturnType Can_GetControllerErrorState(uint8 ctl,Can_ErrorStateType *st){(void)ctl;if(!st)return E_NOT_OK;*st=sim.diag.errorState;return E_OK;}
Std_ReturnType Can_GetDebugStatus(uint8 ctl,Can_DebugStatusType *st){(void)ctl;if(!st)return E_NOT_OK;*st=sim.diag;return E_OK;}
Can_ReturnType Can_Write(Can_HwHandleType hth,const Can_PduType *p){
    (void)hth;
    if(!p || !p->sdu || p->length>8)return CAN_NOT_OK;
    if(sim.fail_next_tx){sim.fail_next_tx=0;sim.dropped_tx++;return CAN_NOT_OK;}
    if(sim.frame_count>=SIM_MAX_FRAMES)return CAN_BUSY;
    SimFrame *f=&sim.frames[sim.frame_count++];
    f->ms=sim.now_ms;f->id=p->id;f->dlc=p->length;
    memcpy(f->payload,p->sdu,p->length);
    return CAN_OK;
}
void Can_MainFunction_BusOff(void){}
void Can_MainFunction_Mode(void){}
void Can_MainFunction_Write(void){}
void Can_MainFunction_Read(void){}
void Can_RxPollIsr(void){}
