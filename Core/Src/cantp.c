/*
 * cantp.c
 *
 *  Created on: Aug 26, 2025
 *      Author: huynh
 */


 #include <stdint.h>
#include "cantp.h"

#include "cantp.h"
#include <string.h>

static CANTP_Session_TypeDef tx_session;
static CANTP_Session_TypeDef rx_session;
static uint8_t rx_buffer[4095];
static uint16_t rx_buffer_len = 0;

extern void CAN_Write(uint32_t id, uint8_t dlc, uint8_t *data);
extern uint8_t CAN_Read(uint32_t *id, uint8_t *dlc, uint8_t *data);


uint32_t HAL_GetTick(void) {

    return 0;
}

void CANTP_Init(void) {
    memset(&tx_session, 0, sizeof(CANTP_Session_TypeDef));
    memset(&rx_session, 0, sizeof(CANTP_Session_TypeDef));
    rx_buffer_len = 0;
}

void CANTP_MainFunction(void) {

    if (tx_session.block_size > 0 && tx_session.transmitted_length < tx_session.payload_length) {
        if (HAL_GetTick() >= tx_session.next_frame_time) {
            uint8_t cf_frame[8];
            cf_frame[0] = (CANTP_FRAME_TYPE_CF | tx_session.consecutive_frame_counter);
            uint8_t data_size = (tx_session.payload_length - tx_session.transmitted_length > 7) ? 7 : (tx_session.payload_length - tx_session.transmitted_length);

            memcpy(&cf_frame[1], tx_session.payload + tx_session.transmitted_length, data_size);
            CAN_Write(tx_session.can_id, 8, cf_frame);

            tx_session.transmitted_length += data_size;
            tx_session.consecutive_frame_counter = (tx_session.consecutive_frame_counter % 0x0F) + 1;
            tx_session.block_size--;

            if (tx_session.block_size == 0 && tx_session.transmitted_length < tx_session.payload_length) {
                tx_session.block_size = 0;
            } else {
                tx_session.next_frame_time = HAL_GetTick() + tx_session.separation_time;
            }
        }
    }


    uint32_t rx_id;
    uint8_t rx_dlc;
    uint8_t rx_data[8];

    if (CAN_Read(&rx_id, &rx_dlc, rx_data)) {
        CANTP_RxIndication(rx_id, rx_data, rx_dlc);
    }
}

void CANTP_Transmit(uint32_t can_id, uint8_t *payload, uint16_t length) {
    if (length <= 7) {
        uint8_t sf_frame[8];
        sf_frame[0] = (CANTP_FRAME_TYPE_SF | length);
        memcpy(&sf_frame[1], payload, length);
        CAN_Write(can_id, length + 1, sf_frame);
    } else {
        uint8_t ff_frame[8];
        ff_frame[0] = (CANTP_FRAME_TYPE_FF | (length >> 8));
        ff_frame[1] = (uint8_t)length;
        memcpy(&ff_frame[2], payload, 6);
        CAN_Write(can_id, 8, ff_frame);

        memcpy(tx_session.payload, payload, length);
        tx_session.can_id = can_id;
        tx_session.payload_length = length;
        tx_session.transmitted_length = 6;
        tx_session.consecutive_frame_counter = 1;
        tx_session.block_size = 0;
    }
}

void CANTP_RxIndication(uint32_t can_id, uint8_t *data, uint8_t dlc) {
    uint8_t frame_type = (data[0] & 0xF0);

    if (frame_type == CANTP_FRAME_TYPE_SF) {
        uint8_t length = data[0] & 0x0F;
        memcpy(rx_buffer, &data[1], length);
        rx_buffer_len = length;
    } else if (frame_type == CANTP_FRAME_TYPE_FF) {
        rx_session.payload_length = ((data[0] & 0x0F) << 8) | data[1];
        memcpy(rx_session.payload, &data[2], 6);
        rx_session.received_length = 6;
        rx_session.consecutive_frame_counter = 1;

        uint8_t fc_frame[8] = {CANTP_FRAME_TYPE_FC | CANTP_FC_FLOW_STATUS_CTS, 0x08, 0x00}; // BS=8, STmin=0
        CAN_Write(can_id, 8, fc_frame);
    } else if (frame_type == CANTP_FRAME_TYPE_CF) {
        uint8_t sn = data[0] & 0x0F;
        if (sn == rx_session.consecutive_frame_counter) {
            uint8_t data_size = (rx_session.payload_length - rx_session.received_length > 7) ? 7 : (rx_session.payload_length - rx_session.received_length);
            memcpy(rx_session.payload + rx_session.received_length, &data[1], data_size);
            rx_session.received_length += data_size;
            rx_session.consecutive_frame_counter = (rx_session.consecutive_frame_counter % 0x0F) + 1;

            if (rx_session.received_length >= rx_session.payload_length) {
                memcpy(rx_buffer, rx_session.payload, rx_session.payload_length);
                rx_buffer_len = rx_session.payload_length;
                memset(&rx_session, 0, sizeof(CANTP_Session_TypeDef));
            }
        }
    } else if (frame_type == CANTP_FRAME_TYPE_FC) {
        uint8_t flow_status = data[0] & 0x0F;
        if (flow_status == CANTP_FC_FLOW_STATUS_CTS) {
            tx_session.block_size = data[1];
            tx_session.separation_time = data[2];
        } else if (flow_status == CANTP_FC_FLOW_STATUS_OVFLW) {

             memset(&tx_session, 0, sizeof(CANTP_Session_TypeDef));
        }
    }
}

uint16_t CANTP_ReadRxData(uint8_t *buffer, uint16_t max_len) {
    if (rx_buffer_len > 0) {
        uint16_t len = (rx_buffer_len > max_len) ? max_len : rx_buffer_len;
        memcpy(buffer, rx_buffer, len);
        rx_buffer_len = 0;
        return len;
    }
    return 0;
}
