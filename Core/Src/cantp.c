/*
 * cantp2222.c
 *
 *  Created on: Sep 11, 2025
 *      Author: Minh
 */
/*
 * cantp.c
 *  Sequence: CAN Driver ISR -> CanTp -> UDS
 */

#include <cantp.h>
#include <stdint.h>
#include <string.h>

// --------- Static Sessions ---------
static CANTP_Session_TypeDef tx_session;
static CANTP_Session_TypeDef rx_session;

// --------- Extern APIs -------------
extern void CAN_Write(uint32_t id, uint8_t dlc, uint8_t *data);
extern uint32_t HAL_GetTick(void);

// --------- UDS Callback ------------
__attribute__((weak)) void UDS_RxIndication(uint32_t can_id, uint8_t *data, uint8_t len) {
    // User override: Application layer (UDS) will implement this
}

// --------- CanTp Init --------------
void CANTP_Init(void) {
    memset(&tx_session, 0, sizeof(tx_session));
    memset(&rx_session, 0, sizeof(rx_session));
}

// --------- Transmit ----------------
void CANTP_Transmit(uint32_t can_id, uint8_t *payload, uint16_t length) {
    if (length <= 7) {
        // Single Frame
        uint8_t sf[8] = {0};
        sf[0] = (CANTP_FRAME_TYPE_SF | length);
        memcpy(&sf[1], payload, length);
        CAN_Transmit(can_id, length + 1, sf);
    } else {
        // First Frame
        uint8_t ff[8] = {0};
        ff[0] = (CANTP_FRAME_TYPE_FF | (length >> 8));
        ff[1] = (uint8_t)length;
        memcpy(&ff[2], payload, 6);
        CAN_Transmit(can_id, 8, ff);

        // Save Tx session
        memset(&tx_session, 0, sizeof(tx_session));
        memcpy(tx_session.payload, payload, length);
        tx_session.can_id = can_id;
        tx_session.payload_length = length;
        tx_session.transmitted_length = 6;
        tx_session.consecutive_frame_counter = 1;
        tx_session.state = CANTP_TX_WAIT_FC;
    }
}

// --------- Rx Indication from CAN Driver ---------
void CANTP_RxIndication(uint32_t can_id, uint8_t *data, uint8_t dlc) {
    uint8_t frame_type = (data[0] & 0xF0);

    if (frame_type == CANTP_FRAME_TYPE_SF) {
        // ---- Single Frame ----
        uint8_t len = data[0] & 0x0F;
        UDS_RxIndication(can_id, &data[1], len);

    } else if (frame_type == CANTP_FRAME_TYPE_FF) {
        // ---- First Frame ----
        rx_session.payload_length = ((data[0] & 0x0F) << 8) | data[1];
        memcpy(rx_session.payload, &data[2], 6);
        rx_session.received_length = 6;
        rx_session.consecutive_frame_counter = 1;

        // Send Flow Control (CTS)
        uint8_t fc[8] = {0};
        fc[0] = (CANTP_FRAME_TYPE_FC | CANTP_FC_FLOW_STATUS_CTS);
        fc[1] = 0x08; // Block Size = 8
        fc[2] = 0x00; // STmin = 0
        CAN_Transmit(can_id, 8, fc);

    } else if (frame_type == CANTP_FRAME_TYPE_CF) {
        // ---- Consecutive Frame ----
        uint8_t sn = data[0] & 0x0F;
        if (sn == rx_session.consecutive_frame_counter) {
            uint8_t size = (rx_session.payload_length - rx_session.received_length > 7) ? 7 : (rx_session.payload_length - rx_session.received_length);
            memcpy(rx_session.payload + rx_session.received_length, &data[1], size);
            rx_session.received_length += size;
            rx_session.consecutive_frame_counter = (rx_session.consecutive_frame_counter % 0x0F) + 1;

            if (rx_session.received_length >= rx_session.payload_length) {
                // ---- Finished ----
                UDS_RxIndication(can_id, rx_session.payload, rx_session.payload_length);
                memset(&rx_session, 0, sizeof(rx_session));
            }
        }

    } else if (frame_type == CANTP_FRAME_TYPE_FC) {
        // ---- Flow Control ----
        uint8_t fs = data[0] & 0x0F;
        if (fs == CANTP_FC_FLOW_STATUS_CTS) {
            tx_session.block_size = data[1];
            tx_session.separation_time = data[2];
            tx_session.next_frame_time = HAL_GetTick();
            tx_session.state = CANTP_TX_IN_PROGRESS;
        } else {
            // Overflow or Wait
            memset(&tx_session, 0, sizeof(tx_session));
        }
    }
}

// --------- MainFunction for Tx CF (called in loop) ---------
void CANTP_MainFunction(void) {
    if (tx_session.state == CANTP_TX_IN_PROGRESS &&
        tx_session.transmitted_length < tx_session.payload_length) {

        if (HAL_GetTick() >= tx_session.next_frame_time) {
            uint8_t cf[8] = {0};
            cf[0] = (CANTP_FRAME_TYPE_CF | tx_session.consecutive_frame_counter);
            uint8_t size = (tx_session.payload_length - tx_session.transmitted_length > 7) ? 7 : (tx_session.payload_length - tx_session.transmitted_length);
            memcpy(&cf[1], tx_session.payload + tx_session.transmitted_length, size);

            CAN_Transmit(tx_session.can_id, size + 1, cf);

            tx_session.transmitted_length += size;
            tx_session.consecutive_frame_counter = (tx_session.consecutive_frame_counter % 0x0F) + 1;

            tx_session.next_frame_time = HAL_GetTick() + tx_session.separation_time;
        }
    }
}


