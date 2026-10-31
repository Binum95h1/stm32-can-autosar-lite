#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "mock.h"
#include "AirbagS.h"
#ifdef TEST_AUTOSAR
#include "ComIf.h"
#include "PduR.h"
#include "CanIf.h"
#else
#include "BareCan.h"
#endif
static void tick(uint32 t) {mock_now=t;AirbagS_MainFunction();}
static void init(void) {
    mock_reset();
#ifdef TEST_AUTOSAR
    CanIf_Init(&CanIf_Config); PduR_Init(&PduR_Config); ComIf_Init();
#else
    BareCan_Init();
#endif
    AirbagS_Init();
    /* Harness: init the actual MPU6050 driver, then zero only the mock counters/time. */
    if (Mpu6050If_Init()!=E_OK) { fputs("driver startup failed\n",stderr); exit(2); }
    mock_reset();
}
static void scenario(const char *s){
    init();
    if(!strcmp(s,"normal")){tick(0);tick(10);tick(20);}
    else if(!strcmp(s,"threshold_below")){mock_axes(10239,0,0);tick(0);}
    else if(!strcmp(s,"threshold_above")){mock_axes(10245,0,0);tick(0);}
    else if(!strcmp(s,"negative_impact")){mock_axes(-10245,0,0);tick(0);}
    else if(!strcmp(s,"diagonal_above")){mock_axes(7500,7500,0);tick(0);}
    else if(!strcmp(s,"diagonal_below")){mock_axes(7200,7200,0);tick(0);}
    else if(!strcmp(s,"z_axis_impact")){mock_axes(0,0,10245);tick(0);}
    else if(!strcmp(s,"latch")){mock_axes(12288,0,0);tick(0);mock_axes(0,0,4096);tick(10);tick(20);}
    else if(!strcmp(s,"sensor_fault")){mock_fail_sensor(TRUE);tick(0);}
    else if(!strcmp(s,"sensor_recovery")){mock_fail_sensor(TRUE);tick(0);mock_fail_sensor(FALSE);tick(40);}
    else if(!strcmp(s,"post_collision_sensor_fault")){mock_axes(12288,0,0);tick(0);mock_fail_sensor(TRUE);tick(10);}
    else if(!strcmp(s,"tx_busy_retry")){mock_fail_tx(TRUE);tick(0);mock_fail_tx(FALSE);tick(10);}
    else if(!strcmp(s,"tx_busy_twice")){mock_fail_tx(TRUE);tick(0);tick(10);mock_fail_tx(FALSE);tick(20);}
    else if(!strcmp(s,"diag_before_100")){tick(0);tick(90);}
    else if(!strcmp(s,"diag_at_100")){tick(0);tick(100);}
    else if(!strcmp(s,"diag_repeated")){tick(100);tick(199);tick(200);}
    else if(!strcmp(s,"tx_seq_increment")){tick(0);tick(10);tick(20);}
    else if(!strcmp(s,"tx_seq_wrap")){for(int j=0;j<257;j++)tick((uint32)j*10U);}
    else if(!strcmp(s,"heartbeat_latched")){mock_axes(12288,0,0);tick(0);tick(10);tick(20);}
    else if(!strcmp(s,"sensor_error_timing")){mock_fail_sensor(TRUE);tick(0);}
    else if(!strcmp(s,"fault_pulse_on")){mock_fail_sensor(TRUE);tick(0);tick(10);tick(20);}
    else if(!strcmp(s,"fault_pulse_off")){mock_fail_sensor(TRUE);tick(0);tick(30);}
    else if(!strcmp(s,"fault_pulse_restart")){mock_fail_sensor(TRUE);tick(0);tick(30);tick(100);}
    else if(!strcmp(s,"fault_recover_early")){mock_fail_sensor(TRUE);tick(0);mock_fail_sensor(FALSE);tick(10);}
    else if(!strcmp(s,"crash_after_fault")){mock_fail_sensor(TRUE);tick(0);mock_fail_sensor(FALSE);mock_axes(12288,0,0);tick(40);}
    else if(!strcmp(s,"fault_after_crash")){mock_axes(12288,0,0);tick(0);mock_fail_sensor(TRUE);tick(10);}
    else if(!strcmp(s,"report_every_10ms")){tick(0);tick(10);tick(20);tick(30);}
    else if(!strcmp(s,"mg_conversion")){
        if(Mpu6050If_RawToMg(4096)!=1000 || Mpu6050If_RawToMg(-4096)!=-1000 ||
           Mpu6050If_RawToMg(10240)!=2500 || Mpu6050If_RawToMg(-32768)!=-8000)exit(1);
    }
    else if(!strcmp(s,"axes_big_endian")){
        Mpu6050If_AxesType ax; mock_axes(-4096,10240,4096);
        if(Mpu6050If_ReadAxes(&ax)!=E_OK || ax.x!=-4096 || ax.y!=10240 || ax.z!=4096)exit(1);
    }
    else if(!strcmp(s,"sensor_init_ok")){if(Mpu6050If_Init()!=E_OK){fprintf(stderr,"sensor init expected success\n");exit(1);}}
    else if(!strcmp(s,"sensor_init_bad_id")){mock_bad_whoami(TRUE);if(Mpu6050If_Init()!=E_NOT_OK){fprintf(stderr,"sensor init expected failure\n");exit(1);}}
    else {fprintf(stderr,"Unknown scenario %s\n",s);exit(2);}
}
int main(int argc,char **argv){
    if(argc!=2){fprintf(stderr,"Usage: runner scenario\n");return 2;}
    scenario(argv[1]);
    printf("{\"scenario\":\"%s\",\"time\":%lu,\"buzzer\":%d,\"buzzer_edges\":%u,\"reads\":%u,\"delay_calls\":%u,\"max_delay\":%u,\"cfg_writes\":%u,\"config\":%u,\"accel_config\":%u,\"frames\":[",argv[1],(unsigned long)mock_now,mock_buzzer, mock_buzzer_edges,mock_reads,mock_delay_calls,mock_max_delay,mock_cfg_writes,mock_last_config,mock_last_accel_config);
    for(unsigned i=0;i<mock_n;i++) {
        MockFrame *f=&mock_frames[i];
        printf("%s{\"t\":%lu,\"id\":%u,\"ok\":%u,\"data\":[",i?",":"",(unsigned long)f->when,f->id,f->ok);
        for(unsigned j=0;j<f->dlc;j++)printf("%s%u",j?",":"",f->data[j]);
        printf("]}");
    }
    puts("]}");return 0;
}
