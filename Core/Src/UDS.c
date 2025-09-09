/*
 * UDS.c
 *
 *  Created on: Sep 9, 2025
 *      Author: Minh
 */


#include "uds.h"
#include "cantp.h"
#include <string.h>

static uint32_t uds_request_id;
static uint32_t uds_response_id;

static uint8_t uds_rx_buffer[256];
static uint16_t uds_rx_length = 0;
static bool uds_new_request = false;

void UDS_Init(uint32_t req_id, uint32_t resp_id)
{
    uds_request_id = req_id;
    uds_response_id = resp_id;
    uds_rx_length = 0;
    uds_new_request = false;
}

void UDS_MainFunction(void)
{
    if (uds_new_request)
    {
        uds_new_request = false;

        uint8_t sid = uds_rx_buffer[0]; // service ID
        switch (sid)
        {
        case UDS_SID_DIAGNOSTIC_SESSION_CONTROL:
            // ví dụ: phản hồi "default session active"
            {
                uint8_t resp[2] = { sid + 0x40, 0x01 };
                UDS_SendPositiveResponse(sid, resp + 1, 1);
            }
            break;

        case UDS_SID_TESTER_PRESENT:
            {
                uint8_t resp[1] = { sid + 0x40 };
                UDS_SendPositiveResponse(sid, NULL, 0);
            }
            break;

        case UDS_SID_ECU_RESET:
            {
                uint8_t resp[2] = { sid + 0x40, 0x01 }; // hard reset ack
                UDS_SendPositiveResponse(sid, resp + 1, 1);
                // TODO: trigger MCU reset
            }
            break;

        case UDS_SID_REQUEST_DOWNLOAD:
            // TODO: implement flash download sequence
            UDS_SendPositiveResponse(sid, NULL, 0);
            break;

        case UDS_SID_TRANSFER_DATA:
            // TODO: handle data block write
            UDS_SendPositiveResponse(sid, NULL, 0);
            break;

        case UDS_SID_REQUEST_TRANSFER_EXIT:
            // TODO: finalize flash programming
            UDS_SendPositiveResponse(sid, NULL, 0);
            break;

        default:
            UDS_SendNegativeResponse(sid, UDS_NRC_SERVICE_NOT_SUPPORTED);
            break;
        }
    }
}

void UDS_RxIndication(uint32_t can_id, uint8_t *data, uint8_t dlc)
{
    if (can_id != uds_request_id) return;

    // copy vào buffer tạm thời
    if (dlc > sizeof(uds_rx_buffer)) return;
    memcpy(uds_rx_buffer, data, dlc);
    uds_rx_length = dlc;
    uds_new_request = true;
}

void UDS_SendPositiveResponse(uint8_t sid, const uint8_t *data, uint16_t len)
{
    uint8_t buffer[64];
    uint16_t total_len = 1 + len;

    buffer[0] = sid + 0x40; // Positive response SID
    if (len > 0 && data != NULL)
    {
        memcpy(&buffer[1], data, len);
    }

    CANTP_Transmit(uds_response_id, buffer, total_len);
}

void UDS_SendNegativeResponse(uint8_t sid, uint8_t nrc)
{
    uint8_t buffer[3];
    buffer[0] = 0x7F;   // Negative Response SID
    buffer[1] = sid;
    buffer[2] = nrc;

    CANTP_Transmit(uds_response_id, buffer, 3);
}
