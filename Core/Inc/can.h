#ifndef __CAN_H
#define __CAN_H

#include <stdint.h>

/* ==================== CAN REGISTERS ==================== */
typedef struct {
    volatile uint32_t MCR;
    volatile uint32_t MSR;
    volatile uint32_t TSR;
    volatile uint32_t RF0R;
    volatile uint32_t RF1R;
    volatile uint32_t IER;
    volatile uint32_t ESR;
    volatile uint32_t BTR;
} CAN_TypeDef;

typedef struct {
    volatile uint32_t TIR;
    volatile uint32_t TDTR;
    volatile uint32_t TDLR;
    volatile uint32_t TDHR;
} CAN_TxMailbox_TypeDef;

typedef struct {
    volatile uint32_t RIR;
    volatile uint32_t RDTR;
    volatile uint32_t RDLR;
    volatile uint32_t RDHR;
} CAN_RxMailbox_TypeDef;

typedef struct {
    volatile uint32_t FR1;
    volatile uint32_t FR2;
} CAN_FilterRegister_TypeDef;

typedef struct {
    volatile uint32_t FMR;   /*!< CAN filter master register          */
    volatile uint32_t FM1R;  /*!< Filter mode register                */
    uint32_t RESERVED0;
    volatile uint32_t FS1R;  /*!< Filter scale register               */
    uint32_t RESERVED1;
    volatile uint32_t FFA1R; /*!< Filter FIFO assignment register     */
    uint32_t RESERVED2;
    volatile uint32_t FA1R;  /*!< Filter activation register          */
    uint32_t RESERVED3[8];
    CAN_FilterRegister_TypeDef sFilterRegister[14]; /*!< 14 filter banks */
} CAN_Filter_TypeDef;

/* ==================== BASE ADDRESSES ==================== */
#define CAN1_BASE             (0x40006400UL)
#define CAN1                  ((CAN_TypeDef *) CAN1_BASE)
#define CAN1_TX_MAILBOX_0     ((CAN_TxMailbox_TypeDef *) (CAN1_BASE + 0x180UL))
#define CAN1_RX_MAILBOX_0     ((CAN_RxMailbox_TypeDef *) (CAN1_BASE + 0x1B0UL))
#define CAN1_RX_MAILBOX_1     ((CAN_RxMailbox_TypeDef *) (CAN1_BASE + 0x1C0UL))

#define CAN_FILTER_BASE       (0x40006600UL)
#define CAN_FILTER            ((CAN_Filter_TypeDef *) CAN_FILTER_BASE)

/* ==================== ENUM & DEFINES ==================== */
typedef enum {
    DISABLE = 0x00,
    ENABLE = 0x01
} FunctionalState;

#define CAN_MODE_NORMAL       0x00
#define CAN_MODE_SILENT       0x01
#define CAN_MODE_LOOPBACK     0x02
#define CAN_MODE_LOOPBACK_SILENT 0x03

/* CAN IER bits */
#define CAN_IER_FMPIE0        ((uint32_t)0x00000002)  // FIFO0 message pending interrupt

/* CAN FMR bits */
#define CAN_FMR_FINIT         ((uint32_t)0x00000001)

/* ==================== API ==================== */
void CAN_Init(uint8_t mode, uint32_t baud_rate_prescaler);
uint8_t CAN_Transmit(uint32_t id, uint8_t dlc, uint8_t *data);
uint8_t CAN_Receive(uint32_t *id, uint8_t *dlc, uint8_t *data);
void CAN_ITConfig(CAN_TypeDef* CANx, uint32_t CAN_IT, FunctionalState NewState);

/* New API for filter configuration */
void CAN_FilterInit(uint8_t filterBank, uint32_t id, uint32_t mask, uint8_t fifoAssign);

#endif // __CAN_H
