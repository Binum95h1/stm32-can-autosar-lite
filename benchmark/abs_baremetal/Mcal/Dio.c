#include "Dio.h"
#include "Reg.h"
void Dio_Init(void)
{
    RCC->APB2ENR |= RCC_APB2ENR_IOPAEN | RCC_APB2ENR_IOPBEN | RCC_APB2ENR_AFIOEN | RCC_APB2ENR_IOPDEN;
    AFIO->MAPR &= ~(7UL<<24); AFIO->MAPR |= (2UL<<24); /* SWD on, JTAG off */
    GPIOA->CRL &= ~((0xFUL<<(4U*4U)) | (0xFUL<<(5U*4U)));
    GPIOA->CRL |=  ((0x2UL<<(4U*4U)) | (0x2UL<<(5U*4U))); /* PA4 PA5 output */
    GPIOB->CRL &= ~((0xFUL<<(0U*4U)) | (0xFUL<<(1U*4U)));
    GPIOB->CRL |=  ((0x2UL<<(0U*4U)) | (0x2UL<<(1U*4U))); /* PB0 PB1 output */
    Dio_WriteMotor1(0,0); Dio_WriteMotor2(0,0);
}
void Dio_WriteMotor1(uint8_t in1, uint8_t in2){ if(in1) GPIOB->BSRR=(1UL<<0); 
    else GPIOB->BRR=(1UL<<0); 
    if(in2) GPIOB->BSRR=(1UL<<1); 
    else GPIOB->BRR=(1UL<<1); }
void Dio_WriteMotor2(uint8_t in1, uint8_t in2)
{ 
    if(in1) GPIOA->BSRR=(1UL<<5); 
    else GPIOA->BRR=(1UL<<5); 
    
    if(in2) GPIOA->BSRR=(1UL<<4); 
    else GPIOA->BRR=(1UL<<4); 
}
