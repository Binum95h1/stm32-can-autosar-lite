#ifndef REG_H
#define REG_H
#include <stdint.h>
#include <stddef.h>
#include "Project_Cfg.h"
/* STM32F103C8T6 / RM0008 register layout. HSE 8 MHz, SYSCLK 72 MHz. */
#define APB1_BASE     0x40000000UL
#define APB2_BASE     0x40010000UL
#define RCC_BASE      0x40021000UL
#define FLASH_R_BASE  0x40022000UL
#define GPIOA_BASE    0x40010800UL
#define GPIOB_BASE    0x40010C00UL
#define AFIO_BASE     0x40010000UL
#define USART2_BASE   0x40004400UL
#define I2C1_BASE     0x40005400UL
#define CAN1_BASE     0x40006400UL
#define ADC1_BASE     0x40012400UL
#define SYSTICK_BASE  0xE000E010UL
#define NVIC_ISER_BASE 0xE000E100UL

typedef struct {
    volatile uint32_t CR, CFGR, CIR, APB2RSTR, APB1RSTR, AHBENR;
    volatile uint32_t APB2ENR, APB1ENR, BDCR, CSR;
} RCC_t;
typedef struct {
    volatile uint32_t ACR, KEYR, OPTKEYR, SR, CR, AR, OBR, WRPR;
} FLASH_t;
typedef struct {
    volatile uint32_t CRL, CRH, IDR, ODR, BSRR, BRR, LCKR;
} GPIO_t;
typedef struct {
    volatile uint32_t EVCR, MAPR, EXTICR[4], MAPR2;
} AFIO_t;
typedef struct {
    volatile uint32_t SR, DR, BRR, CR1, CR2, CR3, GTPR;
} USART_t;
typedef struct {
    volatile uint32_t CR1, CR2, OAR1, OAR2, DR, SR1, SR2, CCR, TRISE;
} I2C_t;
typedef struct {
    volatile uint32_t SR, CR1, CR2, SMPR1, SMPR2;
    volatile uint32_t JOFR1, JOFR2, JOFR3, JOFR4, HTR, LTR, SQR1, SQR2, SQR3, JSQR;
    volatile uint32_t JDR1, JDR2, JDR3, JDR4, DR;
} ADC_t;
typedef struct {
    volatile uint32_t MCR, MSR, TSR, RF0R, RF1R, IER, ESR, BTR;
    uint32_t _reserved0[88];
    volatile uint32_t TI0R, TDT0R, TDL0R, TDH0R;
    volatile uint32_t TI1R, TDT1R, TDL1R, TDH1R;
    volatile uint32_t TI2R, TDT2R, TDL2R, TDH2R;
    volatile uint32_t RI0R, RDT0R, RDL0R, RDH0R;
    volatile uint32_t RI1R, RDT1R, RDL1R, RDH1R;
    uint32_t _reserved1[12];
    volatile uint32_t FMR, FM1R;
    uint32_t _reserved2;
    volatile uint32_t FS1R;
    uint32_t _reserved3;
    volatile uint32_t FFA1R;
    uint32_t _reserved4;
    volatile uint32_t FA1R;
    uint32_t _reserved5[8];
    volatile uint32_t FR[28];
} CAN_t;
typedef struct {
    volatile uint32_t CTRL, LOAD, VAL, CALIB;
} SysTick_t;
#define RCC    ((RCC_t*)RCC_BASE)
#define FLASH  ((FLASH_t*)FLASH_R_BASE)
#define GPIOA  ((GPIO_t*)GPIOA_BASE)
#define GPIOB  ((GPIO_t*)GPIOB_BASE)
#define AFIO   ((AFIO_t*)AFIO_BASE)
#define USART2 ((USART_t*)USART2_BASE)
#define I2C1   ((I2C_t*)I2C1_BASE)
#define ADC1   ((ADC_t*)ADC1_BASE)
#define CAN1   ((CAN_t*)CAN1_BASE)
#define SYSTICK ((SysTick_t*)SYSTICK_BASE)
_Static_assert(offsetof(CAN_t, TI0R) == 0x180, "bxCAN TX offset invalid");
_Static_assert(offsetof(CAN_t, RI1R) == 0x1C0, "bxCAN RX offset invalid");
_Static_assert(offsetof(CAN_t, FMR) == 0x200, "bxCAN filter offset invalid");
_Static_assert(offsetof(CAN_t, FR) == 0x240, "bxCAN FR offset invalid");
_Static_assert(offsetof(ADC_t, DR) == 0x4C, "F103 ADC DR offset invalid");

#define RCC_CR_HSEON         (1UL << 16)
#define RCC_CR_HSERDY        (1UL << 17)
#define RCC_CR_PLLON         (1UL << 24)
#define RCC_CR_PLLRDY        (1UL << 25)
#define RCC_CFGR_SW_PLL      2UL
#define RCC_CFGR_SWS_PLL     (2UL << 2)
#define RCC_APB2ENR_AFIOEN  (1UL << 0)
#define RCC_APB2ENR_IOPAEN  (1UL << 2)
#define RCC_APB2ENR_IOPBEN  (1UL << 3)
#define RCC_APB2ENR_ADC1EN  (1UL << 9)
#define RCC_APB1ENR_USART2EN (1UL << 17)
#define RCC_APB1ENR_I2C1EN  (1UL << 21)
#define RCC_APB1ENR_CAN1EN  (1UL << 25)
#define RCC_APB1RSTR_I2C1RST (1UL << 21)
#define RCC_APB1RSTR_CAN1RST (1UL << 25)
#define FLASH_ACR_PRFTBE     (1UL << 4)
#define FLASH_ACR_LATENCY_2WS 2UL

#define USART_SR_TXE    (1UL << 7)
#define USART_SR_TC     (1UL << 6)
#define USART_CR1_UE    (1UL << 13)
#define USART_CR1_TE    (1UL << 3)
#define USART_CR1_RE    (1UL << 2)
#define I2C_CR1_PE      (1UL << 0)
#define I2C_CR1_START   (1UL << 8)
#define I2C_CR1_STOP    (1UL << 9)
#define I2C_CR1_ACK     (1UL << 10)
#define I2C_CR1_SWRST   (1UL << 15)
#define I2C_SR1_SB      (1UL << 0)
#define I2C_SR1_ADDR    (1UL << 1)
#define I2C_SR1_BTF     (1UL << 2)
#define I2C_SR1_TXE     (1UL << 7)
#define I2C_SR1_AF      (1UL << 10)
#define I2C_SR2_BUSY    (1UL << 1)
#define ADC_SR_EOC      (1UL << 1)
#define ADC_CR2_ADON    (1UL << 0)
#define ADC_CR2_CAL     (1UL << 2)
#define ADC_CR2_RSTCAL  (1UL << 3)
#define ADC_CR2_EXTTRIG (1UL << 20)
#define ADC_CR2_SWSTART (1UL << 22)
#define CAN_MCR_INRQ    (1UL << 0)
#define CAN_MCR_SLEEP   (1UL << 1)
#define CAN_MCR_NART    (1UL << 4)
#define CAN_MCR_ABOM    (1UL << 6)
#define CAN_MSR_INAK    (1UL << 0)
#define CAN_MSR_SLAK    (1UL << 1)
#define CAN_TSR_TME0    (1UL << 26)
#define CAN_TSR_TME1    (1UL << 27)
#define CAN_TSR_TME2    (1UL << 28)
#define CAN_TIR_TXRQ    (1UL << 0)
#define CAN_RF1R_FMP1   3UL
#define CAN_RF1R_RFOM1  (1UL << 5)
#define CAN_IER_FMPIE1  (1UL << 4)
#define CAN_FMR_FINIT   (1UL << 0)
#define CAN_RI1R_STID_S 21U
#define CAN_RI1R_IDE    (1UL << 2)
#define CAN_TSR_RQCP0   (1UL << 0)
#define CAN_TSR_TXOK0   (1UL << 1)
#define CAN_TSR_ALST0   (1UL << 2)
#define CAN_TSR_TERR0   (1UL << 3)
#define CAN_TSR_ABRQ0   (1UL << 7)
#define CAN_TSR_RQCP1   (1UL << 8)
#define CAN_TSR_TXOK1   (1UL << 9)
#define CAN_TSR_ALST1   (1UL << 10)
#define CAN_TSR_TERR1   (1UL << 11)
#define CAN_TSR_ABRQ1   (1UL << 15)
#define CAN_TSR_RQCP2   (1UL << 16)
#define CAN_TSR_TXOK2   (1UL << 17)
#define CAN_TSR_ALST2   (1UL << 18)
#define CAN_TSR_TERR2   (1UL << 19)
#define CAN_TSR_ABRQ2   (1UL << 23)
#define CAN1_RX1_IRQn   21U
#define SYSTICK_ENABLE  (1UL << 0)
#define SYSTICK_TICKINT (1UL << 1)
#define SYSTICK_CLKSRC  (1UL << 2)
#define I2C_TIMEOUT     200000UL
#define ADC_TIMEOUT     100000UL
#define CAN_TIMEOUT     100000UL
#define UART_TIMEOUT    100000UL
#define LCD_I2C_7BIT    0x27U
void nvic_enable(uint32_t irq);
#endif
