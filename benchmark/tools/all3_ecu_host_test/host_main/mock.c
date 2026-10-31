#include "mock.h"
#include "Can.h"
#include "Can_Cfg.h"
#include "Adc.h"
#include "Dio.h"
#include "Det.h"
#include "LcdIf.h"
#include "OsIf.h"
#include "Can_UpperLayer.h"
#include "Project_Cfg.h"
#include <string.h>
uint32 mock_ms = 0U;
uint32 mock_adc = 0U;
uint8 mock_brake = 0U;
uint8 mock_direction = 0U;
int mock_fail_next_tx = 0;
MockFrame mock_frames[64];
int mock_frame_count = 0;
void Mock_Reset(void)
{
    mock_ms=0;mock_adc=0;mock_brake=0;mock_direction=0;
    mock_fail_next_tx=0;mock_frame_count=0;
    memset(mock_frames,0,sizeof(mock_frames));
}
void Mock_Inject(uint16 id, const uint8 *d, uint8 dlc)
{
    Can_UpperRxIndication(CAN_MAIN_CONTROLLER, id, d, dlc);
}
uint32 OsIf_GetTimeMs(void){return mock_ms;}
void OsIf_DelayMs(uint32 ms){mock_ms += ms;}
uint32 Adc_Read(void){return mock_adc;}
uint8 Dio_ReadChannel(Dio_ChannelType ch){return ch==DIO_CHANNEL_BRAKE_SW?mock_brake:mock_direction;}
void LcdIf_Init(void){}
void LcdIf_Clear(void){}
void LcdIf_SetCursor(int row,int col){(void)row;(void)col;}
void LcdIf_SendString(const char *s){(void)s;}
void Det_Init(void){}
void Det_Start(void){}
Std_ReturnType Det_ReportError(uint16 m,uint8 i,uint8 a,uint8 e){(void)m;(void)i;(void)a;(void)e;return E_OK;}
void Det_Print(const char *s){(void)s;}
void Det_Printf(const char *fmt,...){(void)fmt;}
Can_ReturnType Can_Write(Can_HwHandleType h,const Can_PduType *p)
{
    MockFrame *f;
    (void)h;
    if(mock_fail_next_tx){mock_fail_next_tx=0;return CAN_BUSY;}
    if(!p||p->length>8||!p->sdu||mock_frame_count>=64)return CAN_NOT_OK;
    f=&mock_frames[mock_frame_count++];
    f->id=p->id;f->dlc=p->length;
    memcpy(f->data,p->sdu,p->length);
    return CAN_OK;
}
Std_ReturnType Can_GetDebugStatus(uint8 controller,Can_DebugStatusType *s)
{
    if(controller!=CAN_MAIN_CONTROLLER||!s)return E_NOT_OK;
    memset(s,0,sizeof(*s));
    s->controllerMode=CAN_CS_STARTED;
    s->errorState=CAN_ERRORSTATE_ACTIVE;
    return E_OK;
}
Std_ReturnType Can_SetControllerMode(uint8 controller,Can_ControllerStateType mode){(void)controller;(void)mode;return E_OK;}
Std_ReturnType Can_GetControllerMode(uint8 controller,Can_ControllerStateType *mode){(void)controller;if(!mode)return E_NOT_OK;*mode=CAN_CS_STARTED;return E_OK;}
Std_ReturnType Can_GetControllerErrorState(uint8 controller,Can_ErrorStateType *state){(void)controller;if(!state)return E_NOT_OK;*state=CAN_ERRORSTATE_ACTIVE;return E_OK;}
void Can_MainFunction_BusOff(void){}
void Can_MainFunction_Mode(void){}
void Can_MainFunction_Write(void){}
void Can_MainFunction_Read(void){}
