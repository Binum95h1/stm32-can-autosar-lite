#ifndef PDUR_CFG_H
#define PDUR_CFG_H

#define PDUR_PDU_ABS_TO_MAIN       (0U)
#define PDUR_PDU_CAN_DIAG_STATUS   (1U)
#define PDUR_TX_ROUTE_COUNT        (2U)

#define PDUR_PDU_MAIN_TO_ABS       (0U)
#define PDUR_RX_ROUTE_COUNT        (1U)

#define PDUR_MODULE_ID             ((uint16)51U)
#define PDUR_INSTANCE_ID           ((uint8)0U)
#define PDUR_SID_INIT              ((uint8)0x00U)
#define PDUR_SID_COM_TRANSMIT      ((uint8)0x89U)
#define PDUR_E_UNINIT              ((uint8)0x01U)
#define PDUR_E_PDU_ID_INVALID      ((uint8)0x02U)
#define PDUR_E_PARAM_POINTER       ((uint8)0x03U)

#endif /* PDUR_CFG_H */
