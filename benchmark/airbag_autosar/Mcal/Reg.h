#ifndef REG_H
#define REG_H
#include <stdint.h>

#define PERIPH_BASE         0x40000000UL
#define APB1_BASE           (PERIPH_BASE + 0x00000000UL)
#define APB2_BASE           (PERIPH_BASE + 0x00010000UL)
#define AHB_BASE            (PERIPH_BASE + 0x00020000UL)
#define USART2_BASE         (APB1_BASE + 0x00004400UL)
#define I2C1_BASE           (APB1_BASE + 0x00005400UL)
#define CAN1_BASE           (APB1_BASE + 0x00006400UL)
#define GPIOA_BASE          (APB2_BASE + 0x00000800UL)
#define GPIOB_BASE          (APB2_BASE + 0x00000C00UL)
#define AFIO_BASE           (APB2_BASE + 0x00000000UL)
#define RCC_BASE            (AHB_BASE  + 0x00001000UL)
#define FLASH_BASE          (AHB_BASE  + 0x00002000UL)
#define SYSTICK_BASE        0xE000E010UL

typedef struct {
    volatile uint32_t CR;
    volatile uint32_t CFGR;
    volatile uint32_t CIR;
    volatile uint32_t APB2RSTR;
    volatile uint32_t APB1RSTR;
    volatile uint32_t AHBENR;
    volatile uint32_t APB2ENR;
    volatile uint32_t APB1ENR;
    volatile uint32_t BDCR;
    volatile uint32_t CSR;
} RCC_t;

typedef struct {
    volatile uint32_t ACR;
} FLASH_t;

typedef struct {
    volatile uint32_t CRL;
    volatile uint32_t CRH;
    volatile uint32_t IDR;
    volatile uint32_t ODR;
    volatile uint32_t BSRR;
    volatile uint32_t BRR;
    volatile uint32_t LCKR;
} GPIO_t;

typedef struct {
    volatile uint32_t EVCR;
    volatile uint32_t MAPR;
    volatile uint32_t EXTICR[4];
    volatile uint32_t RESERVED;
    volatile uint32_t MAPR2;
} AFIO_t;

typedef struct {
    volatile uint32_t SR;
    volatile uint32_t DR;
    volatile uint32_t BRR;
    volatile uint32_t CR1;
    volatile uint32_t CR2;
    volatile uint32_t CR3;
    volatile uint32_t GTPR;
} USART_t;

typedef struct {
    volatile uint32_t CR1;
    volatile uint32_t CR2;
    volatile uint32_t OAR1;
    volatile uint32_t OAR2;
    volatile uint32_t DR;
    volatile uint32_t SR1;
    volatile uint32_t SR2;
    volatile uint32_t CCR;
    volatile uint32_t TRISE;
} I2C_t;


typedef struct {
    volatile uint32_t MCR;
    volatile uint32_t MSR;
    volatile uint32_t TSR;
    volatile uint32_t RF0R;
    volatile uint32_t RF1R;
    volatile uint32_t IER;
    volatile uint32_t ESR;
    volatile uint32_t BTR;
    uint32_t RESERVED0[88];
    volatile uint32_t TI0R;
    volatile uint32_t TDT0R;
    volatile uint32_t TDL0R;
    volatile uint32_t TDH0R;
    volatile uint32_t TI1R;
    volatile uint32_t TDT1R;
    volatile uint32_t TDL1R;
    volatile uint32_t TDH1R;
    volatile uint32_t TI2R;
    volatile uint32_t TDT2R;
    volatile uint32_t TDL2R;
    volatile uint32_t TDH2R;
    volatile uint32_t RI0R;
    volatile uint32_t RDT0R;
    volatile uint32_t RDL0R;
    volatile uint32_t RDH0R;
    volatile uint32_t RI1R;
    volatile uint32_t RDT1R;
    volatile uint32_t RDL1R;
    volatile uint32_t RDH1R;
    uint32_t RESERVED1[12];
    volatile uint32_t FMR;
    volatile uint32_t FM1R;
    uint32_t RESERVED2;
    volatile uint32_t FS1R;
    uint32_t RESERVED3;
    volatile uint32_t FFA1R;
    uint32_t RESERVED4;
    volatile uint32_t FA1R;
    uint32_t RESERVED5[8];
    volatile uint32_t FR[28];
} CAN_t;

typedef struct {
    volatile uint32_t CTRL;
    volatile uint32_t LOAD;
    volatile uint32_t VAL;
    volatile uint32_t CALIB;
} SysTick_t;

#define RCC     ((RCC_t *)RCC_BASE)
#define FLASH   ((FLASH_t *)FLASH_BASE)
#define GPIOA   ((GPIO_t *)GPIOA_BASE)
#define GPIOB   ((GPIO_t *)GPIOB_BASE)
#define AFIO    ((AFIO_t *)AFIO_BASE)
#define USART2  ((USART_t *)USART2_BASE)
#define I2C1    ((I2C_t *)I2C1_BASE)
#define CAN1    ((CAN_t *)CAN1_BASE)
#define SYSTICK ((SysTick_t *)SYSTICK_BASE)

#endif
