#include "mock.h"
#include "MainNodeS.h"
#include "MainNodeS_Internal.h"
#include "Project_Cfg.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#ifdef TEST_AUTOSAR
#include "ComIf.h"
#include "PduR.h"
#include "CanIf.h"
static void stack_init(void){CanIf_Init(&CanIf_Config);PduR_Init(&PduR_Config);ComIf_Init();}
#else
#include "BareCan.h"
static void stack_init(void){BareCan_Init();}
#endif
#define CHECK(expr) do {if(!(expr)){fprintf(stderr,"FAIL line %d: %s\n",__LINE__,#expr);return 1;}}while(0)
static int nth_id(uint16 id,int index)
{
    int seen=0;
    for(int i=0;i<mock_frame_count;i++) {
        if(mock_frames[i].id==id){if(seen==index)return i;seen++;}
    }
    return -1;
}
static void tick(uint32 t){mock_ms=t;MainNodeS_MainFunction();}
static void inject_abs(uint8 w1,uint8 w2,uint8 state,uint8 seq)
{
    uint8 d[4]={w1,w2,state,seq};Mock_Inject(CAN_ID_ABS_TO_MAIN,d,4);
}
static void inject_airbag(uint8 deploy,uint8 seq)
{
    uint8 d[2]={deploy,seq};Mock_Inject(CAN_ID_AIRBAG_TO_MAIN,d,2);
}
int main(int argc,char **argv)
{
    const char *mode;
    const MockFrame *f;
    int i;
    if(argc!=2)return 2;
    mode=argv[1];
    Mock_Reset();stack_init();MainNodeS_Init();
    if(strcmp(mode,"normal")==0){
        mock_adc=0x345;tick(10);i=nth_id(0x121,0);CHECK(i>=0);f=&mock_frames[i];
        CHECK(f->dlc==5&&f->data[0]==0x45&&f->data[1]==3&&f->data[2]==0&&f->data[3]==0&&f->data[4]==0);
    }else if(strcmp(mode,"brake")==0){
        mock_adc=0x321;mock_brake=1;tick(10);i=nth_id(0x121,0);CHECK(i>=0);f=&mock_frames[i];
        CHECK(f->data[0]==0x21&&f->data[1]==3&&f->data[2]==1);
    }else if(strcmp(mode,"reverse")==0){
        mock_adc=100;mock_direction=1;tick(10);i=nth_id(0x121,0);CHECK(i>=0);f=&mock_frames[i];CHECK(f->data[3]==1);
    }else if(strcmp(mode,"emergency")==0){
        mock_adc=800;inject_airbag(1,0);tick(10);i=nth_id(0x121,0);CHECK(i>=0);f=&mock_frames[i];
        CHECK(emergency_stop==1&&f->data[0]==0&&f->data[1]==0&&f->data[2]==1);
    }else if(strcmp(mode,"recovery")==0){
        mock_adc=800;inject_airbag(1,0);tick(10);inject_airbag(0,1);tick(20);
        i=nth_id(0x121,1);CHECK(i>=0);f=&mock_frames[i];
        CHECK(emergency_stop==0&&f->data[0]==(800&255)&&f->data[1]==(800>>8)&&f->data[2]==0);
    }else if(strcmp(mode,"abs_rx")==0){
        inject_abs(45,55,2,7);tick(10);
        CHECK(main_abs_rx_count==1&&abs1==45&&abs2==55&&abs3==2&&main_abs_last_seq==7);
    }else if(strcmp(mode,"abs_gap")==0){
        inject_abs(40,50,1,7);tick(10);inject_abs(41,51,1,9);tick(20);
        CHECK(main_abs_rx_count==2&&main_abs_lost_count==1&&main_abs_last_seq==9);
    }else if(strcmp(mode,"airbag_gap")==0){
        inject_airbag(0,0);tick(10);inject_airbag(0,2);tick(20);
        CHECK(main_airbag_rx_count==2&&main_airbag_lost_count==1&&main_airbag_last_seq==2);
    }else if(strcmp(mode,"ignore_unknown")==0){
        uint8 wrong[2]={1,1};
        Mock_Inject(0x333,wrong,2);tick(10);
        CHECK(main_abs_rx_count==0&&main_airbag_rx_count==0&&emergency_stop==0);
    }else if(strcmp(mode,"ignore_short")==0){
        uint8 wrong[2]={33,44};
        Mock_Inject(0x122,wrong,2);tick(10);CHECK(main_abs_rx_count==0);
    }else if(strcmp(mode,"tx_busy_retry")==0){
        mock_adc=90;mock_fail_next_tx=1;tick(10);
        CHECK(main_to_abs_tx_count==0&&main_to_abs_seq==0);
        tick(20);i=nth_id(0x121,0);CHECK(i>=0);CHECK(mock_frames[i].data[4]==0&&main_to_abs_tx_count==1);
    }else if(strcmp(mode,"diagnostic")==0){
        tick(100);i=nth_id(0x211,0);CHECK(i>=0);
        CHECK(mock_frames[i].dlc==8&&mock_frames[i].data[7]==CAN_CS_STARTED);
    }else if(strcmp(mode,"adc_mask")==0){
        mock_adc=0x1FFF;tick(10);i=nth_id(0x121,0);CHECK(i>=0);
        CHECK(mock_frames[i].data[0]==0xFF&&mock_frames[i].data[1]==0x0F);
    }else if(strcmp(mode,"concurrent_rx")==0){
        inject_abs(35,46,2,0);inject_airbag(1,4);tick(10);
        CHECK(abs1==35&&abs2==46&&emergency_stop==1&&main_abs_rx_count==1&&main_airbag_rx_count==1);
    }else if(strcmp(mode,"seq_wrap")==0){
        inject_abs(20,30,1,255);tick(10);inject_abs(21,31,1,0);tick(20);
        CHECK(main_abs_lost_count==0&&main_abs_last_seq==0);
    }else if(strcmp(mode,"seq_duplicate")==0){
        inject_abs(20,30,1,0);tick(10);inject_abs(20,30,1,0);tick(20);
        CHECK(main_abs_rx_count==2&&main_abs_lost_count==0&&main_abs_last_seq==0);
    }else if(strcmp(mode,"seq_stale")==0){
        inject_abs(20,30,1,10);tick(10);inject_abs(20,30,1,9);tick(20);
        CHECK(main_abs_lost_count==0&&main_abs_last_seq==10);
    }else if(strcmp(mode,"airbag_duplicate")==0){
        inject_airbag(0,1);tick(10);inject_airbag(0,1);tick(20);
        CHECK(main_airbag_lost_count==0&&main_airbag_last_seq==1);
    }else if(strcmp(mode,"airbag_stale")==0){
        inject_airbag(0,4);tick(10);inject_airbag(0,3);tick(20);
        CHECK(main_airbag_lost_count==0&&main_airbag_last_seq==4);
    }else {fprintf(stderr,"Unknown scenario %s\n",mode);return 2;}
    printf("%s|frames=%d|tx=%lu|next=%u|absrx=%lu|abslost=%lu|abslast=%u|airrx=%lu|airlost=%lu|airlast=%u|emergency=%u|abs1=%u|abs2=%u|abs3=%u|lcd=%s\n",
           mode,mock_frame_count,(unsigned long)main_to_abs_tx_count,main_to_abs_seq,
           (unsigned long)main_abs_rx_count,(unsigned long)main_abs_lost_count,main_abs_last_seq,
           (unsigned long)main_airbag_rx_count,(unsigned long)main_airbag_lost_count,main_airbag_last_seq,
           emergency_stop,abs1,abs2,abs3,lcd_row0);
    for(i=0;i<mock_frame_count;i++){
        const MockFrame *p=&mock_frames[i];
        printf("TX:%03X:%u",p->id,p->dlc);
        for(int j=0;j<p->dlc;j++)printf(":%02X",p->data[j]);
        printf("\n");
    }
    return 0;
}
