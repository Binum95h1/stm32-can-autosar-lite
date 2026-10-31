#include "sim.h"
#include "AbsNodeS.h"
#include "MotorIf.h"
#include "Project_Cfg.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#ifdef HOST_BARE
#include "BareCan.h"
#include "Can_UpperLayer.h"
#else
#include "ComIf.h"
#include "CanIf.h"
#include "PduR.h"
#endif

static void inject(uint16_t id,uint16_t adc,uint8_t brake,uint8_t direction,uint8_t sequence,uint8_t len){
    uint8_t p[8]={adc&255,(adc>>8)&15,brake,direction,sequence,0,0,0};
#ifdef HOST_BARE
    Can_UpperRxIndication(0,id,p,len);
#else
    CanIf_RxIndication(0,id,p,len);
#endif
}
static void step(void){sim_advance_10ms();AbsNodeS_MainFunction();}
static void until(uint32_t ms){while(sim.now_ms<ms)step();}
static int count_frames(uint16_t id){int n=0;for(int i=0;i<sim.frame_count;i++)if(sim.frames[i].id==id)n++;return n;}
static const SimFrame *last_frame(uint16_t id){for(int i=sim.frame_count-1;i>=0;i--)if(sim.frames[i].id==id)return &sim.frames[i];return NULL;}
static int first_frame_time(uint16_t id){for(int i=0;i<sim.frame_count;i++)if(sim.frames[i].id==id)return (int)sim.frames[i].ms;return -1;}
static int last_logged_lost(void){
    const char *p=sim.log,*last=NULL;
    while((p=strstr(p," | LOST="))!=NULL){last=p+8;p+=8;}
    return last?atoi(last):-1;
}
static void emit(const char *scenario,int p300,int p310,int frame_count_t10){
    const SimFrame *last=last_frame(CAN_ID_ABS_TO_MAIN);
    const SimFrame *diag=last_frame(CAN_ID_CAN_DIAG_STATUS);
    const char *lost=strstr(sim.log,"LOST=1");
    printf("{\"scenario\":\"%s\",\"variant\":\"%s\",\"now_ms\":%u,\"pwm\":[%u,%u],\"dir\":[[%u,%u],[%u,%u]],\"encoder\":[%u,%u],\"pwm_t300\":%d,\"pwm_t310\":%d,\"frames_t10\":%d,\"feedback_count\":%d,\"feedback_first_ms\":%d,\"diag_count\":%d,\"tx_dropped\":%d,\"saw_lost_one\":%s,\"last_rx_lost\":%d,\"frames\":[",
        scenario,
#ifdef HOST_BARE
        "bare",
#else
        "autosar",
#endif
        (unsigned)sim.now_ms,sim.pwm[0],sim.pwm[1],sim.dir[0][0],sim.dir[0][1],sim.dir[1][0],sim.dir[1][1],sim.encoder[0],sim.encoder[1],p300,p310,frame_count_t10,count_frames(CAN_ID_ABS_TO_MAIN),first_frame_time(CAN_ID_ABS_TO_MAIN),count_frames(CAN_ID_CAN_DIAG_STATUS),sim.dropped_tx,lost?"true":"false",last_logged_lost());
    for(int i=0;i<sim.frame_count;i++){
        const SimFrame *f=&sim.frames[i];
        printf("%s{\"t\":%u,\"id\":%u,\"data\":[",i?",":"",(unsigned)f->ms,f->id);
        for(int j=0;j<f->dlc;j++)printf("%s%u",j?",":"",f->payload[j]);
        printf("]}");
    }
    printf("],\"last_feedback_state\":%d,\"last_feedback_wheels\":[%d,%d],\"last_diag_boff\":%d}\n",
       last?last->payload[2]:-1,last?last->payload[0]:-1,last?last->payload[1]:-1,diag?diag->payload[5]:-1);
}
int main(int argc,char **argv){
    if(argc!=2){fprintf(stderr,"usage: %s scenario\n",argv[0]);return 2;}
    const char *scenario=argv[1];
    int p300=-1,p310=-1,frames_t10=-1;
    sim_reset();
    MotorIf_Init();
#ifdef HOST_BARE
    BareCan_Init();
#else
    CanIf_Init(&CanIf_Config);PduR_Init(&PduR_Config);ComIf_Init();
#endif
    AbsNodeS_Init();
    if(strcmp(scenario,"normal")==0){inject(0x121,0,0,0,0,5);AbsNodeS_MainFunction();until(100);}
    else if(strcmp(scenario,"forward")==0){inject(0x121,4095,0,0,1,5);AbsNodeS_MainFunction();until(100);}
    else if(strcmp(scenario,"reverse")==0){inject(0x121,3072,0,1,1,5);AbsNodeS_MainFunction();until(100);}
    else if(strcmp(scenario,"brake")==0){inject(0x121,4095,1,0,1,5);AbsNodeS_MainFunction();until(100);}
    else if(strcmp(scenario,"wrong_id")==0){inject(0x321,4095,0,0,1,5);AbsNodeS_MainFunction();until(100);}
    else if(strcmp(scenario,"short_dlc")==0){inject(0x121,4095,0,0,1,3);AbsNodeS_MainFunction();until(100);}
    else if(strcmp(scenario,"deadband")==0){inject(0x121,5,0,0,1,5);AbsNodeS_MainFunction();until(100);}
    else if(strcmp(scenario,"feedback_cycle")==0){inject(0x121,1000,0,0,0,5);AbsNodeS_MainFunction();until(10);frames_t10=count_frames(0x122);until(100);}
    else if(strcmp(scenario,"timeout_boundary")==0){inject(0x121,2000,0,0,1,5);AbsNodeS_MainFunction();until(300);p300=sim.pwm[0];until(310);p310=sim.pwm[0];}
    else if(strcmp(scenario,"recovery")==0){inject(0x121,2000,0,0,1,5);AbsNodeS_MainFunction();until(310);p310=sim.pwm[0];inject(0x121,2000,0,0,2,5);AbsNodeS_MainFunction();until(400);}
    else if(strcmp(scenario,"encoder")==0){inject(0x121,4095,0,0,1,5);AbsNodeS_MainFunction();until(100);}
    else if(strcmp(scenario,"diag")==0){
        sim.diag.lec=3;sim.diag.tec=5;sim.diag.rec=2;sim.diag.ewgf=1;sim.diag.epvf=0;sim.diag.boff=1;
        for(int t=0;t<=500;t+=10){if(t%100==0)inject(0x121,0,0,0,(uint8_t)(t/100),5);AbsNodeS_MainFunction();if(t<500)sim_advance_10ms();}
    }
    else if(strcmp(scenario,"sequence_gap")==0){inject(0x121,1000,0,0,0,5);AbsNodeS_MainFunction();step();inject(0x121,1000,0,0,2,5);AbsNodeS_MainFunction();until(100);}
    else if(strcmp(scenario,"first_seq_nonzero")==0){inject(0x121,1000,0,0,7,5);AbsNodeS_MainFunction();until(100);}
    else if(strcmp(scenario,"tx_failure")==0){inject(0x121,0,0,0,0,5);AbsNodeS_MainFunction();sim.fail_next_tx=1;until(200);}
    else if(strcmp(scenario,"motor_stall")==0){sim.motor_stalled=1;for(int t=0;t<=500;t+=10){if(t%100==0)inject(0x121,4095,0,0,(uint8_t)(t/100),5);AbsNodeS_MainFunction();if(t<500)sim_advance_10ms();}}
    else if(strcmp(scenario,"latest_command")==0){inject(0x121,4095,0,0,1,5);inject(0x121,0,1,0,2,5);AbsNodeS_MainFunction();until(100);}
    else {fprintf(stderr,"unknown scenario: %s\n",scenario);return 2;}
    emit(scenario,p300,p310,frames_t10);return 0;
}
