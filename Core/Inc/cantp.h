/*
 * cantp2222.h
 *
 *  Created on: Sep 11, 2025
 *      Author: Minh
 */

#ifndef INC_CANTP_H_
#define INC_CANTP_H_

#include <stdint.h>
#include "can.h"

// --------- Frame Type (Nibble high of PCI byte) ---------
#define CANTP_FRAME_TYPE_SF   0x00  // Single Frame
#define CANTP_FRAME_TYPE_FF   0x10  // First Frame
#define CANTP_FRAME_TYPE_CF   0x20  // Consecutive Frame
#define CANTP_FRAME_TYPE_FC   0x30  // Flow Control

// --------- Flow Control Status (Nibble low of PCI byte) ---------
#define CANTP_FC_FLOW_STATUS_CTS     0x00  // Clear To Send
#define CANTP_FC_FLOW_STATUS_WAIT    0x01  // Wait
#define CANTP_FC_FLOW_STATUS_OVFLW   0x02  // Overflow

// --------- Tx Session State ---------
typedef enum {
    CANTP_TX_IDLE = 0,
    CANTP_TX_WAIT_FC,
    CANTP_TX_IN_PROGRESS
} CANTP_TxStateType;

// --------- Session Structure ---------
typedef struct {
    uint32_t can_id;               // CAN Identifier
    uint16_t payload_length;       // Total payload length
    uint16_t transmitted_length;   // Already transmitted length
    uint16_t received_length;      // Already received length

    uint8_t payload[4095];         // Buffer for payload
    uint8_t consecutive_frame_counter;
    uint8_t block_size;
    uint8_t separation_time;       // STmin
    uint32_t next_frame_time;      // Next CF schedule

    CANTP_TxStateType state;       // Tx state machine
} CANTP_Session_TypeDef;

// --------- API Prototypes ---------
void CANTP_Init(void);
void CANTP_MainFunction(void);  // Must be called periodically

void CANTP_Transmit(uint32_t can_id, uint8_t *payload, uint16_t length);
void CANTP_RxIndication(uint32_t can_id, uint8_t *data, uint8_t dlc);

// --------- Callback to UDS Layer ---------
void UDS_RxIndication(uint32_t can_id, uint8_t *data, uint8_t len);



#endif /* INC_CANTP_H_ */
