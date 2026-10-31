#include "MotorIf.h"
#include "Pwm_Encoder.h"
#include "Dio.h"

void MotorIf_Init(void)
{
    PwmEncoder_Init();
    MotorIf_SetPwm(0U, 0U);
    MotorIf_SetDirection1(0U, 0U);
    MotorIf_SetDirection2(0U, 0U);
}

void MotorIf_SetPwm(uint16 ccr1, uint16 ccr2){
    Pwm_SetMotor(ccr1, ccr2);
}

uint16 MotorIf_GetPwmPeriod(void){
    return Pwm_GetPeriod();
}

uint16 MotorIf_GetEncoderCount1(void){
    return Encoder_GetCount1();
}

uint16 MotorIf_GetEncoderCount2(void){
    return Encoder_GetCount2();
}

void MotorIf_SetDirection1(uint8 in1, uint8 in2){
    Dio_WriteMotor1(in1, in2);
}

void MotorIf_SetDirection2(uint8 in1, uint8 in2){
    Dio_WriteMotor2(in1, in2);
}
