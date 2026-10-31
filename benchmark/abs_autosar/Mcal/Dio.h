#ifndef DIO_H
#define DIO_H
#include <stdint.h>
void Dio_Init(void);
void Dio_WriteMotor1(uint8_t in1, uint8_t in2);
void Dio_WriteMotor2(uint8_t in1, uint8_t in2);
#endif
