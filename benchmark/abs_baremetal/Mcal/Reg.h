#ifndef REG_H
#define REG_H
#include <stdint.h>

typedef struct
{
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
} RCC_TypeDef;

typedef struct
{
    volatile uint32_t ACR;
    volatile uint32_t KEYR;
    volatile uint32_t OPTKEYR;
    volatile uint32_t SR;
    volatile uint32_t CR;
    volatile uint32_t AR;
    volatile uint32_t OBR;
    volatile uint32_t WRPR;
} FLASH_TypeDef;

typedef struct
{
    volatile uint32_t CRL;
    volatile uint32_t CRH;
    volatile uint32_t IDR;
    volatile uint32_t ODR;
    volatile uint32_t BSRR;
    volatile uint32_t BRR;
    volatile uint32_t LCKR;
} GPIO_TypeDef;

typedef struct
{
    volatile uint32_t EVCR;
    volatile uint32_t MAPR;
    volatile uint32_t EXTICR[4];
    volatile uint32_t MAPR2;
} AFIO_TypeDef;

typedef struct
{
    volatile uint32_t SR;
    volatile uint32_t DR;
    volatile uint32_t BRR;
    volatile uint32_t CR1;
    volatile uint32_t CR2;
    volatile uint32_t CR3;
    volatile uint32_t GTPR;
} USART_TypeDef;

typedef struct
{
    volatile uint32_t CR1;
    volatile uint32_t CR2;
    volatile uint32_t SMCR;
    volatile uint32_t DIER;
    volatile uint32_t SR;
    volatile uint32_t EGR;
    volatile uint32_t CCMR1;
    volatile uint32_t CCMR2;
    volatile uint32_t CCER;
    volatile uint32_t CNT;
    volatile uint32_t PSC;
    volatile uint32_t ARR;
    volatile uint32_t RCR;
    volatile uint32_t CCR1;
    volatile uint32_t CCR2;
    volatile uint32_t CCR3;
    volatile uint32_t CCR4;
    volatile uint32_t BDTR;
    volatile uint32_t DCR;
    volatile uint32_t DMAR;
} TIM1_TypeDef;

typedef struct
{
    volatile uint32_t CR1;
    volatile uint32_t CR2;
    volatile uint32_t SMCR;
    volatile uint32_t DIER;
    volatile uint32_t SR;
    volatile uint32_t EGR;
    volatile uint32_t CCMR1;
    volatile uint32_t CCMR2;
    volatile uint32_t CCER;
    volatile uint32_t CNT;
    volatile uint32_t PSC;
    volatile uint32_t ARR;
    uint32_t RESERVED;
    volatile uint32_t CCR1;
    volatile uint32_t CCR2;
    volatile uint32_t CCR3;
    volatile uint32_t CCR4;
    volatile uint32_t DCR;
    volatile uint32_t DMAR;
} TIM_TypeDef;

typedef struct
{
    volatile uint32_t ISER[8];
    uint32_t RESERVED0[24];

    volatile uint32_t ICER[8];
    uint32_t RESERVED1[24];

    volatile uint32_t ISPR[8];
    uint32_t RESERVED2[24];

    volatile uint32_t ICPR[8];
    uint32_t RESERVED3[24];

    volatile uint32_t IABR[8];
    uint32_t RESERVED4[56];

    volatile uint8_t IP[240];
} NVIC_TypeDef;

typedef struct
{
    volatile uint32_t CTRL;
    volatile uint32_t LOAD;
    volatile uint32_t VAL;
    volatile uint32_t CALIB;
} SysTick_TypeDef;

typedef struct
{
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

    struct
    {
        volatile uint32_t FR1;
        volatile uint32_t FR2;
    } sFilterRegister[14];

} CAN_TypeDef;

#define PERIPH_BASE 0x40000000UL
#define APB1PERIPH_BASE PERIPH_BASE
#define APB2PERIPH_BASE (PERIPH_BASE + 0x10000UL)
#define AHBPERIPH_BASE  (PERIPH_BASE + 0x20000UL)
#define FLASH_R_BASE    0x40022000UL
#define RCC_BASE        (AHBPERIPH_BASE + 0x1000UL)
#define AFIO_BASE       (APB2PERIPH_BASE + 0x0000UL)
#define GPIOA_BASE      (APB2PERIPH_BASE + 0x0800UL)
#define GPIOB_BASE      (APB2PERIPH_BASE + 0x0C00UL)
#define GPIOD_BASE      (APB2PERIPH_BASE + 0x1400UL)
#define USART2_BASE     (APB1PERIPH_BASE + 0x4400UL)
#define TIM2_BASE       (APB1PERIPH_BASE + 0x0000UL)
#define TIM3_BASE       (APB1PERIPH_BASE + 0x0400UL)
#define TIM1_BASE       (APB2PERIPH_BASE + 0x2C00UL)
#define CAN1_BASE       (APB1PERIPH_BASE + 0x6400UL)
#define NVIC_BASE       0xE000E100UL
#define SYSTICK_BASE    0xE000E010UL

#define RCC     ((RCC_TypeDef*)RCC_BASE)
#define FLASH   ((FLASH_TypeDef*)FLASH_R_BASE)
#define AFIO    ((AFIO_TypeDef*)AFIO_BASE)
#define GPIOA   ((GPIO_TypeDef*)GPIOA_BASE)
#define GPIOB   ((GPIO_TypeDef*)GPIOB_BASE)
#define GPIOD   ((GPIO_TypeDef*)GPIOD_BASE)
#define USART2  ((USART_TypeDef*)USART2_BASE)
#define TIM1    ((TIM1_TypeDef*)TIM1_BASE)
#define TIM2    ((TIM_TypeDef*)TIM2_BASE)
#define TIM3    ((TIM_TypeDef*)TIM3_BASE)
#define CAN1    ((CAN_TypeDef*)CAN1_BASE)
#define NVIC    ((NVIC_TypeDef*)NVIC_BASE)
#define SYSTICK ((SysTick_TypeDef*)SYSTICK_BASE)

#define RCC_APB2ENR_AFIOEN (1UL<<0)
#define RCC_APB2ENR_IOPAEN (1UL<<2)
#define RCC_APB2ENR_IOPBEN (1UL<<3)
#define RCC_APB2ENR_IOPDEN (1UL<<5)
#define RCC_APB2ENR_TIM1EN (1UL<<11)
#define RCC_APB1ENR_TIM2EN (1UL<<0)
#define RCC_APB1ENR_TIM3EN (1UL<<1)
#define RCC_APB1ENR_USART2EN (1UL<<17)
#define RCC_APB1ENR_CAN1EN (1UL<<25)
#define RCC_APB1RSTR_CAN1RST (1UL<<25)

#define CAN_MCR_INRQ (1UL<<0)
#define CAN_MCR_SLEEP (1UL<<1)
#define CAN_MCR_ABOM (1UL<<6)
#define CAN_MSR_INAK (1UL<<0)
#define CAN_MSR_SLAK (1UL<<1)
#define CAN_TSR_RQCP0 (1UL<<0)
#define CAN_TSR_TXOK0 (1UL<<1)
#define CAN_TSR_ALST0 (1UL<<2)
#define CAN_TSR_TERR0 (1UL<<3)
#define CAN_TSR_RQCP1 (1UL<<8)
#define CAN_TSR_TXOK1 (1UL<<9)
#define CAN_TSR_ALST1 (1UL<<10)
#define CAN_TSR_TERR1 (1UL<<11)
#define CAN_TSR_RQCP2 (1UL<<16)
#define CAN_TSR_TXOK2 (1UL<<17)
#define CAN_TSR_ALST2 (1UL<<18)
#define CAN_TSR_TERR2 (1UL<<19)
#define CAN_TSR_TME0 (1UL<<26)
#define CAN_TSR_TME1 (1UL<<27)
#define CAN_TSR_TME2 (1UL<<28)
#define CAN_IER_FMPIE1 (1UL<<4)
#define CAN_IER_ERRIE  (1UL<<15)
#define CAN_IER_LECIE  (1UL<<11)
#define CAN_RF1R_FMP1  (3UL<<0)
#define CAN_RF1R_RFOM1 (1UL<<5)
#define CAN_RI1R_IDE   (1UL<<2)
#define CAN_RI1R_STID_S 21U
#define CAN_TIR_TXRQ (1UL<<0)

#define TIM_CR1_CEN (1UL<<0)
#define TIM_CCMR_OC1M_PWM1 (6UL<<4)
#define TIM_CCMR_OC1PE (1UL<<3)
#define TIM_CCMR_OC2M_PWM1 (6UL<<12)
#define TIM_CCMR_OC2PE (1UL<<11)
#define TIM_CCER_CC1E (1UL<<0)
#define TIM_CCER_CC2E (1UL<<4)
#define TIM_BDTR_MOE (1UL<<15)
#define TIM_SMCR_SMS_ENCODER_TI12 3UL
#define TIM_CCMR1_CC1S_TI1 1UL
#define TIM_CCMR1_CC2S_TI2 (1UL<<8)

#define USART_SR_TXE (1UL<<7)
#define USART_SR_TC (1UL<<6)
#define USART_CR1_UE (1UL<<13)
#define USART_CR1_TE (1UL<<3)
#define USART_CR1_RE (1UL<<2)

#define USB_LP_CAN1_RX0_IRQn 20U
#define CAN1_RX1_IRQn 21U
#define SysTick_IRQn 15U

#endif
