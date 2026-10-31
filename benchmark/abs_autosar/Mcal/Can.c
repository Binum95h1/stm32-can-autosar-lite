#include "Can.h"
#include "Reg.h"
#include "SchM.h"
#include "Det.h"
#include "CanIf.h"
#include "Project_Cfg.h"

#define CAN_MCR_NART       (1UL << 4)
#define CAN_ESR_EWGF       (1UL << 0)
#define CAN_ESR_EPVF       (1UL << 1)
#define CAN_ESR_BOFF       (1UL << 2)

static Can_ControllerStateType Can_ControllerMode = CAN_CS_UNINIT;
static const Can_ConfigType *Can_ConfigPtr = NULL_PTR;
static boolean Can_BusOffReported = FALSE;

#define CAN_START_SEC_CODE
#include "MemMap.h"

static Std_ReturnType Can_WaitSet(volatile uint32 *reg, uint32 mask)
{
    uint32 timeout = CAN_TIMEOUT_CNT;
    while (((*reg) & mask) == 0U) {
        if (--timeout == 0U) {
            return E_NOT_OK;
        }
    }
    return E_OK;
}

static Std_ReturnType Can_WaitClear(volatile uint32 *reg, uint32 mask)
{
    uint32 timeout = CAN_TIMEOUT_CNT;
    while (((*reg) & mask) != 0U) {
        if (--timeout == 0U) {
            return E_NOT_OK;
        }
    }
    return E_OK;
}

static Can_ErrorStateType Can_ReadErrorState(void)
{
    uint32 esr = CAN1->ESR;

    if ((esr & CAN_ESR_BOFF) != 0U) {
        return CAN_ERRORSTATE_BUSOFF;
    }
    if ((esr & CAN_ESR_EPVF) != 0U) {
        return CAN_ERRORSTATE_PASSIVE;
    }
    return CAN_ERRORSTATE_ACTIVE;
}

static void Can_ConfigGpioAndFilter(void)
{
    RCC->APB2ENR |= RCC_APB2ENR_IOPAEN | RCC_APB2ENR_AFIOEN;

    /* CAN1 on PA11/PA12: PA11 RX floating input, PA12 TX alternate push-pull. */
    GPIOA->CRH &= ~((0xFUL << 12U) | (0xFUL << 16U));
    GPIOA->CRH |=  ((0x4UL << 12U) | (0xBUL << 16U));

    CAN1->FMR |= 1UL;
    CAN1->FA1R &= ~(1UL << 0U);
    CAN1->FS1R &= ~(1UL << 0U);       /* 16-bit filter scale. */
    CAN1->FM1R |=  (1UL << 0U);       /* Identifier-list mode. */
    CAN1->FFA1R |= (1UL << 0U);       /* FIFO1. */
    CAN1->sFilterRegister[0].FR1 = ((uint32)(CAN_ID_MAIN_TO_ABS << 5U) << 16U);
    CAN1->sFilterRegister[0].FR2 = 0U;
    CAN1->FA1R |= (1UL << 0U);
    CAN1->FMR &= ~1UL;

    CAN1->IER |= CAN_IER_FMPIE1 | CAN_IER_ERRIE | CAN_IER_LECIE;
    NVIC->ISER[CAN1_RX1_IRQn / 32U] = (1UL << (CAN1_RX1_IRQn % 32U));
}

void Can_Init(const Can_ConfigType *ConfigPtr)
{
    const Can_ConfigType *config = ConfigPtr;

    if (config == NULL_PTR) {
        config = &Can_Config;
    }

    Can_ConfigPtr = config;

    RCC->APB1ENR |= RCC_APB1ENR_CAN1EN;
    RCC->APB1RSTR |= RCC_APB1RSTR_CAN1RST;
    RCC->APB1RSTR &= ~RCC_APB1RSTR_CAN1RST;

    CAN1->MCR &= ~CAN_MCR_SLEEP;
    (void)Can_WaitClear(&CAN1->MSR, CAN_MSR_SLAK);

    CAN1->MCR |= CAN_MCR_INRQ;
    (void)Can_WaitSet(&CAN1->MSR, CAN_MSR_INAK);

    CAN1->MCR |= CAN_MCR_ABOM;
    CAN1->MCR &= ~CAN_MCR_NART;       /* retransmission enabled */
    CAN1->BTR = config->bitTimingRegisterValue;

    Can_ConfigGpioAndFilter();

    Can_ControllerMode = CAN_CS_STOPPED;
    Can_BusOffReported = FALSE;
}

void Can_DeInit(void)
{
    if (Can_ControllerMode == CAN_CS_UNINIT) {
        return;
    }

    CAN1->MCR |= CAN_MCR_INRQ;
    (void)Can_WaitSet(&CAN1->MSR, CAN_MSR_INAK);
    CAN1->MCR |= CAN_MCR_SLEEP;
    (void)Can_WaitSet(&CAN1->MSR, CAN_MSR_SLAK);

    Can_ConfigPtr = NULL_PTR;
    Can_ControllerMode = CAN_CS_UNINIT;
}

Std_ReturnType Can_SetControllerMode(uint8 Controller, Can_ControllerStateType Transition)
{
    if (Controller != CAN_MAIN_CONTROLLER) {
        (void)Det_ReportError(CAN_MODULE_ID, CAN_INSTANCE_ID, CAN_SID_SET_CONTROLLER_MODE, CAN_E_PARAM_CONTROLLER);
        return E_NOT_OK;
    }

    if (Can_ConfigPtr == NULL_PTR) {
        (void)Det_ReportError(CAN_MODULE_ID, CAN_INSTANCE_ID, CAN_SID_SET_CONTROLLER_MODE, CAN_E_UNINIT);
        return E_NOT_OK;
    }

    switch (Transition) {
    case CAN_CS_STARTED:
        CAN1->MCR &= ~CAN_MCR_SLEEP;
        if (Can_WaitClear(&CAN1->MSR, CAN_MSR_SLAK) != E_OK) {
            return E_NOT_OK;
        }
        CAN1->MCR &= ~CAN_MCR_INRQ;
        if (Can_WaitClear(&CAN1->MSR, CAN_MSR_INAK) != E_OK) {
            return E_NOT_OK;
        }
        Can_ControllerMode = CAN_CS_STARTED;
        Can_BusOffReported = FALSE;
        return E_OK;

    case CAN_CS_STOPPED:
        CAN1->MCR |= CAN_MCR_INRQ;
        if (Can_WaitSet(&CAN1->MSR, CAN_MSR_INAK) != E_OK) {
            return E_NOT_OK;
        }
        Can_ControllerMode = CAN_CS_STOPPED;
        return E_OK;

    case CAN_CS_SLEEP:
        CAN1->MCR |= CAN_MCR_SLEEP;
        if (Can_WaitSet(&CAN1->MSR, CAN_MSR_SLAK) != E_OK) {
            return E_NOT_OK;
        }
        Can_ControllerMode = CAN_CS_SLEEP;
        return E_OK;

    default:
        (void)Det_ReportError(CAN_MODULE_ID, CAN_INSTANCE_ID, CAN_SID_SET_CONTROLLER_MODE, CAN_E_TRANSITION);
        return E_NOT_OK;
    }
}

Std_ReturnType Can_GetControllerMode(uint8 Controller, Can_ControllerStateType *ControllerModePtr)
{
    if (ControllerModePtr == NULL_PTR) {
        (void)Det_ReportError(CAN_MODULE_ID, CAN_INSTANCE_ID, CAN_SID_GET_CONTROLLER_MODE, CAN_E_PARAM_POINTER);
        return E_NOT_OK;
    }
    if (Controller != CAN_MAIN_CONTROLLER) {
        (void)Det_ReportError(CAN_MODULE_ID, CAN_INSTANCE_ID, CAN_SID_GET_CONTROLLER_MODE, CAN_E_PARAM_CONTROLLER);
        return E_NOT_OK;
    }

    *ControllerModePtr = Can_ControllerMode;
    return E_OK;
}

Std_ReturnType Can_GetControllerErrorState(uint8 Controller, Can_ErrorStateType *ErrorStatePtr)
{
    if (ErrorStatePtr == NULL_PTR) {
        (void)Det_ReportError(CAN_MODULE_ID, CAN_INSTANCE_ID, CAN_SID_GET_ERROR_STATE, CAN_E_PARAM_POINTER);
        return E_NOT_OK;
    }
    if (Controller != CAN_MAIN_CONTROLLER) {
        (void)Det_ReportError(CAN_MODULE_ID, CAN_INSTANCE_ID, CAN_SID_GET_ERROR_STATE, CAN_E_PARAM_CONTROLLER);
        return E_NOT_OK;
    }
    if (Can_ControllerMode == CAN_CS_UNINIT) {
        (void)Det_ReportError(CAN_MODULE_ID, CAN_INSTANCE_ID, CAN_SID_GET_ERROR_STATE, CAN_E_UNINIT);
        return E_NOT_OK;
    }

    *ErrorStatePtr = Can_ReadErrorState();
    return E_OK;
}

Std_ReturnType Can_GetDebugStatus(uint8 Controller, Can_DebugStatusType *StatusPtr)
{
    uint32 esr;
    uint32 msr;

    if (StatusPtr == NULL_PTR) {
        return E_NOT_OK;
    }
    if (Controller != CAN_MAIN_CONTROLLER) {
        return E_NOT_OK;
    }

    esr = CAN1->ESR;
    msr = CAN1->MSR;

    StatusPtr->lec = (uint8)((esr >> 4U) & 0x7U);
    StatusPtr->tec = (uint8)((esr >> 16U) & 0xFFU);
    StatusPtr->rec = (uint8)((esr >> 24U) & 0xFFU);
    StatusPtr->ewgf = (uint8)(((esr & CAN_ESR_EWGF) != 0U) ? 1U : 0U);
    StatusPtr->epvf = (uint8)(((esr & CAN_ESR_EPVF) != 0U) ? 1U : 0U);
    StatusPtr->boff = (uint8)(((esr & CAN_ESR_BOFF) != 0U) ? 1U : 0U);
    StatusPtr->errorState = Can_ReadErrorState();
    StatusPtr->controllerMode = ((msr & CAN_MSR_SLAK) != 0U) ? CAN_CS_SLEEP :
                                (((msr & CAN_MSR_INAK) != 0U) ? CAN_CS_STOPPED : Can_ControllerMode);

    return E_OK;
}

Can_ReturnType Can_Write(Can_HwHandleType Hth, const Can_PduType *PduInfo)
{
    volatile uint32 *tir;
    volatile uint32 *tdtr;
    volatile uint32 *tdlr;
    volatile uint32 *tdhr;
    uint32 low = 0U;
    uint32 high = 0U;

    (void)Hth;

    if (Can_ControllerMode == CAN_CS_UNINIT) {
        (void)Det_ReportError(CAN_MODULE_ID, CAN_INSTANCE_ID, CAN_SID_WRITE, CAN_E_UNINIT);
        return CAN_NOT_OK;
    }

    if ((PduInfo == NULL_PTR) || (PduInfo->sdu == NULL_PTR)) {
        (void)Det_ReportError(CAN_MODULE_ID, CAN_INSTANCE_ID, CAN_SID_WRITE, CAN_E_PARAM_POINTER);
        return CAN_NOT_OK;
    }

    if (PduInfo->length > 8U) {
        (void)Det_ReportError(CAN_MODULE_ID, CAN_INSTANCE_ID, CAN_SID_WRITE, CAN_E_PARAM_DATA_LENGTH);
        return CAN_NOT_OK;
    }

    if (Can_ControllerMode != CAN_CS_STARTED) {
        return CAN_NOT_OK;
    }

    if ((CAN1->TSR & CAN_TSR_TME0) != 0U) {
        tir = &CAN1->TI0R;
        tdtr = &CAN1->TDT0R;
        tdlr = &CAN1->TDL0R;
        tdhr = &CAN1->TDH0R;
    } else if ((CAN1->TSR & CAN_TSR_TME1) != 0U) {
        tir = &CAN1->TI1R;
        tdtr = &CAN1->TDT1R;
        tdlr = &CAN1->TDL1R;
        tdhr = &CAN1->TDH1R;
    } else if ((CAN1->TSR & CAN_TSR_TME2) != 0U) {
        tir = &CAN1->TI2R;
        tdtr = &CAN1->TDT2R;
        tdlr = &CAN1->TDL2R;
        tdhr = &CAN1->TDH2R;
    } else {
        return CAN_BUSY;
    }

    for (uint8 i = 0U; (i < PduInfo->length) && (i < 4U); i++) {
        low |= ((uint32)PduInfo->sdu[i] << (8U * i));
    }
    for (uint8 i = 4U; i < PduInfo->length; i++) {
        high |= ((uint32)PduInfo->sdu[i] << (8U * (i - 4U)));
    }

    SchM_Enter_Can_EXCLUSIVE_AREA_0();
    *tir = ((uint32)PduInfo->id << 21U);
    *tdtr = PduInfo->length;
    *tdlr = low;
    *tdhr = high;
    *tir |= CAN_TIR_TXRQ;
    SchM_Exit_Can_EXCLUSIVE_AREA_0();

    return CAN_OK;
}

void Can_RxPollIsr(void)
{
    while ((CAN1->RF1R & CAN_RF1R_FMP1) != 0U) {
        uint32 ri = CAN1->RI1R;
        uint32 rdt = CAN1->RDT1R;
        uint32 rl = CAN1->RDL1R;
        uint32 rh = CAN1->RDH1R;
        uint16 id = (uint16)((ri >> CAN_RI1R_STID_S) & 0x7FFU);
        uint8 ide = ((ri & CAN_RI1R_IDE) != 0U) ? 1U : 0U;
        uint8 dlc = (uint8)(rdt & 0x0FU);
        uint8 d[8];

        d[0] = (uint8)(rl & 0xFFU);
        d[1] = (uint8)((rl >> 8U) & 0xFFU);
        d[2] = (uint8)((rl >> 16U) & 0xFFU);
        d[3] = (uint8)((rl >> 24U) & 0xFFU);
        d[4] = (uint8)(rh & 0xFFU);
        d[5] = (uint8)((rh >> 8U) & 0xFFU);
        d[6] = (uint8)((rh >> 16U) & 0xFFU);
        d[7] = (uint8)((rh >> 24U) & 0xFFU);

        CAN1->RF1R |= CAN_RF1R_RFOM1;

        if (ide == 0U) {
            CanIf_RxIndication(CAN_MAIN_CONTROLLER, id, d, dlc);
        }
    }
}

void CAN1_RX1_IRQHandler(void)
{
    Can_RxPollIsr();
}

void USB_LP_CAN1_RX0_IRQHandler(void)
{
    /* FIFO0 is unused in this ABS node configuration. */
}

void Can_MainFunction_BusOff(void)
{
    if (Can_ControllerMode == CAN_CS_UNINIT) {
        return;
    }

    if ((CAN1->ESR & CAN_ESR_BOFF) != 0U) {
        if (Can_BusOffReported == FALSE) {
            Can_BusOffReported = TRUE;
            Det_Print("CAN BUS-OFF detected\r\n");
        }
    } else {
        Can_BusOffReported = FALSE;
    }
}

void Can_MainFunction_Mode(void)
{
    /* Mode transitions are executed synchronously in this educational driver. */
}

void Can_MainFunction_Write(void)
{
    /* Direct mailbox transmit: no software Tx queue in this educational driver. */
}

void Can_MainFunction_Read(void)
{
    Can_RxPollIsr();
}

#define CAN_STOP_SEC_CODE
#include "MemMap.h"
