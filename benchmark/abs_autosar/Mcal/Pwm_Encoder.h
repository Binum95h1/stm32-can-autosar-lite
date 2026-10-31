#ifndef PWM_ENCODER_H
#define PWM_ENCODER_H
#include <stdint.h>
void PwmEncoder_Init(void);
void Pwm_SetMotor(uint16_t ccr1, uint16_t ccr2);
uint16_t Pwm_GetPeriod(void);
uint16_t Encoder_GetCount1(void);
uint16_t Encoder_GetCount2(void);
#endif
