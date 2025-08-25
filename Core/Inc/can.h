#ifndef __CAN_H
#define __CAN_H

#include <stdint.h>

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
    volatile uint32_t TDL;
    volatile uint32_t TDH;
} CAN_TxMailbox_TypeDef;

typedef struct {
    volatile uint32_t RIR;
    volatile uint32_t RDTR;
    volatile uint32_t RDL;
    volatile uint32_t RDH;
} CAN_RxMailbox_TypeDef;

#define CAN1_BASE             (0x40006400UL)
#define CAN1                  ((CAN_TypeDef *) CAN1_BASE)
#define CAN1_TX_MAILBOX_0     ((CAN_TxMailbox_TypeDef *) (CAN1_BASE + 0x180UL))
#define CAN1_RX_MAILBOX_0     ((CAN_RxMailbox_TypeDef *) (CAN1_BASE + 0x1B0UL))
#define CAN1_RX_MAILBOX_1     ((CAN_RxMailbox_TypeDef *) (CAN1_BASE + 0x1C0UL))
#define CAN_MODE_NORMAL       0x00
#define CAN_MODE_SILENT       0x01
#define CAN_MODE_LOOPBACK     0x02
#define CAN_MODE_LOOPBACK_SILENT 0x03

void CAN_Init(uint8_t mode, uint32_t baud_rate_prescaler);
uint8_t CAN_Transmit(uint32_t id, uint8_t dlc, uint8_t *data);
uint8_t CAN_Receive(uint32_t *id, uint8_t *dlc, uint8_t *data);

#endif // __CAN_H
