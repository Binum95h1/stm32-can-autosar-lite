#include "Pwm_Encoder.h"
#include "Reg.h"
#define TIM1_ARR_VALUE 35995U
void PwmEncoder_Init(void)
{
    RCC->APB2ENR |= RCC_APB2ENR_IOPAEN | RCC_APB2ENR_TIM1EN | RCC_APB2ENR_AFIOEN;
    RCC->APB1ENR |= RCC_APB1ENR_TIM2EN | RCC_APB1ENR_TIM3EN;
    /* TIM1 CH1 PA8, CH2 PA9: AF push-pull 2MHz */
    GPIOA->CRH &= ~((0xFUL<<0) | (0xFUL<<4));
    GPIOA->CRH |=  ((0xAUL<<0) | (0xAUL<<4));
    TIM1->PSC = 0; 
    TIM1->ARR = TIM1_ARR_VALUE; 
    TIM1->CCR1 = 0; 
    TIM1->CCR2 = 0;
    TIM1->CCMR1 = TIM_CCMR_OC1M_PWM1 | TIM_CCMR_OC1PE | TIM_CCMR_OC2M_PWM1 | TIM_CCMR_OC2PE;
    TIM1->CCER = TIM_CCER_CC1E | TIM_CCER_CC2E;
    TIM1->BDTR = TIM_BDTR_MOE;
    TIM1->EGR = 1; 
    TIM1->CR1 = TIM_CR1_CEN;
    /* TIM2 encoder PA0 PA1 floating input */
    GPIOA->CRL &= ~((0xFUL<<0) | (0xFUL<<4) | (0xFUL<<24) | (0xFUL<<28));
    GPIOA->CRL |=  ((0x4UL<<0) | (0x4UL<<4) | (0x4UL<<24) | (0x4UL<<28));
    TIM2->PSC=0; 
    TIM2->ARR=65535U; 
    TIM2->CCMR1 = TIM_CCMR1_CC1S_TI1 | TIM_CCMR1_CC2S_TI2; 
    TIM2->SMCR = TIM_SMCR_SMS_ENCODER_TI12; 
    TIM2->CCER=TIM_CCER_CC1E|TIM_CCER_CC2E; 
    TIM2->CR1=TIM_CR1_CEN;
    TIM3->PSC=0; 
    TIM3->ARR=65535U; 
    TIM3->CCMR1 = TIM_CCMR1_CC1S_TI1 | TIM_CCMR1_CC2S_TI2; 
    TIM3->SMCR = TIM_SMCR_SMS_ENCODER_TI12; 
    TIM3->CCER=TIM_CCER_CC1E|TIM_CCER_CC2E; 
    TIM3->CR1=TIM_CR1_CEN;
}
void Pwm_SetMotor(uint16_t ccr1, uint16_t ccr2){ 
    TIM1->CCR1=ccr1; 
    TIM1->CCR2=ccr2; 
}
uint16_t Pwm_GetPeriod(void){ 
    return (uint16_t)TIM1->ARR; 
}
uint16_t Encoder_GetCount1(void){ 
    return (uint16_t)TIM2->CNT; 
}
uint16_t Encoder_GetCount2(void){ 
    return (uint16_t)TIM3->CNT; 
}

