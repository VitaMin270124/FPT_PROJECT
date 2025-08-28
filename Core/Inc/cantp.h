/*
 * cantp.h
 *
 *  Created on: Aug 26, 2025
 *      Author: huynh
 */

#ifndef INC_CANTP_H_
#define INC_CANTP_H_

#include <stdint.h>
#include"can.h"

#define CANTP_FRAME_TYPE_SF 0x00 // Single Frame
#define CANTP_FRAME_TYPE_FF 0x10 // First Frame
#define CANTP_FRAME_TYPE_CF 0x20 // Consecutive Frame
#define CANTP_FRAME_TYPE_FC 0x30 // Flow Control

// Định nghĩa trạng thái Flow Control
#define CANTP_FC_FLOW_STATUS_CTS 0x00
#define CANTP_FC_FLOW_STATUS_WAIT 0x01
#define CANTP_FC_FLOW_STATUS_OVFLW 0x02

// Định nghĩa một phiên CANTP
typedef struct {
	 uint32_t can_id;
	 uint8_t payload[4095];
	 uint16_t payload_length;
	    uint16_t transmitted_length;
	    uint16_t received_length;
	    uint8_t consecutive_frame_counter;
	    uint8_t block_size;
	    uint8_t separation_time;
	    uint8_t tx_pending_block_size;
	    uint32_t next_frame_time;
} CANTP_Session_TypeDef;

void CANTP_Init(void);
void CANTP_MainFunction(void);
void CANTP_Transmit(uint32_t can_id, uint8_t *payload, uint16_t length);
void CANTP_RxIndication(uint32_t can_id, uint8_t *data, uint8_t dlc);



#endif /* INC_CANTP_H_ */
