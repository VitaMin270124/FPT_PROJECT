/*
 * UDS.c
 *
 *  Created on: Sep 9, 2025
 *      Author: Minh
 */
#include "UDS.h"
#include <string.h>

// Static variables
static UDS_SessionType uds_active_session;

static uint32_t uds_request_id;
static uint32_t uds_response_id;

static uint8_t uds_rx_buffer[256];
static uint16_t uds_rx_length = 0;

static uint32_t download_address;
static uint32_t download_size;
static uint32_t bytes_received;
static uint8_t  block_counter;

static bool uds_new_request = false;

// Static Functions
static void UDS_HandleDiagnosticSessionControl(uint8_t *req, uint16_t len);
static void UDS_HandleRequestDownload(uint8_t *reqData, uint16_t reqLen);
static void UDS_HandleTransferData(uint8_t *reqData, uint16_t reqLen);
static void UDS_HandleTransferExit(uint8_t *reqData, uint16_t reqLen);

// Global variables
bool update_done = false;
uint32_t receivedCRC = 0;

// Global typedef
FlashPartitionId_t update_partition;

// Global Functions
extern void CANTP_Transmit(uint32_t can_id, uint8_t *payload, uint16_t length);

void UDS_Init()
{
	uds_active_session = UDS_SESSION_DEFAULT;   // <-- Init to Default Session
	update_done = 0;
	receivedCRC = 0;
    uds_request_id = 0x7E0;
    uds_response_id = 0x7E8;
    uds_rx_length = 0;
    download_address   = 0;
    download_size      = 0;
    bytes_received     = 0;
    block_counter      = 0;
    uds_new_request = false;
}


UDS_SessionType UDS_GetCurrentSession(void)
{
    return uds_active_session;
}

static void UDS_HandleDiagnosticSessionControl(uint8_t *req, uint16_t len)
{
    if (len < 2) {
        UDS_SendNegativeResponse(UDS_SID_DIAGNOSTIC_SESSION_CONTROL,
                                 UDS_NRC_INCORRECT_MESSAGE_LENGTH);
        return;
    }

    uint8_t subFunction = req[1];

    switch (subFunction)
    {
    case UDS_SESSION_DEFAULT:
        uds_active_session = UDS_SESSION_DEFAULT;
        UDS_SendPositiveResponse(UDS_SID_DIAGNOSTIC_SESSION_CONTROL,
                                 &subFunction, 1);
        break;

    case UDS_SESSION_PROGRAMMING:
        uds_active_session = UDS_SESSION_PROGRAMMING;
        UDS_SendPositiveResponse(UDS_SID_DIAGNOSTIC_SESSION_CONTROL,
                                 &subFunction, 1);
        break;

    case UDS_SESSION_EXTENDED:
        uds_active_session = UDS_SESSION_EXTENDED;
        UDS_SendPositiveResponse(UDS_SID_DIAGNOSTIC_SESSION_CONTROL,
                                 &subFunction, 1);
        break;

    default:
        UDS_SendNegativeResponse(UDS_SID_DIAGNOSTIC_SESSION_CONTROL,
                                 UDS_NRC_SUBFUNCTION_NOT_SUPPORTED);
        break;
    }
}

static void UDS_HandleRequestDownload(uint8_t *reqData, uint16_t reqLen)
{
    if (reqLen >= 5) {
        uint8_t dataFormatId = reqData[1];        // thường = 0x00
        uint8_t addrLenFmtId = reqData[2];        // nibble thấp = addrLen, nibble cao = sizeLen

        uint8_t addrLen = addrLenFmtId & 0x0F;    // số byte cho address
        uint8_t sizeLen = (addrLenFmtId >> 4) & 0x0F; // số byte cho size

        // Kiểm tra đủ dữ liệu trong request
        if (reqLen < (3 + addrLen + sizeLen)) {
               UDS_SendNegativeResponse(uds_rx_buffer[0], 0x13);
               return;
        }

        // Parse memoryAddress
        download_address = 0;
        for (uint8_t i = 0; i < addrLen; i++) {
            download_address = (download_address << 8) | reqData[3 + i];
        }

        // Parse memorySize
        download_size = 0;
        for (uint8_t i = 0; i < sizeLen; i++) {
            download_size = (download_size << 8) | reqData[3 + addrLen + i];
        }

        if(download_size > 24*1024) {
        	UDS_SendNegativeResponse(reqData[0], 0x13); // Incorrect Length
        	return;
        }

        bytes_received = 0;
        block_counter = 1;

        if(0x08004000 <= download_address < 0x08010000)
        	update_partition = PARTITION_APP_MAIN;
        else if(0x08010000 <= download_address < 0x0801C000)
        	update_partition = PARTITION_APP_BACKUP;
        else {
        	UDS_SendNegativeResponse(reqData[0], 0x31); // REQUEST_OUT_OF_RANGE (ngoài vùng bộ nhớ được ghi)
        	return;
        }

        FlashManager_ErasePartition(update_partition);

        uint8_t resp[2] = {0x02, 0x00}; // MaxBlockLength=0x0200 = 512 bytes
        UDS_SendPositiveResponse(reqData[0], resp, 2);
    } else {
        UDS_SendNegativeResponse(reqData[0], 0x13); // Incorrect Length
    }
    return;
}

static void UDS_HandleTransferData(uint8_t *reqData, uint16_t reqLen)
{
    if (reqLen > 2) {
        uint8_t blockNum = reqData[1];
        if (blockNum != block_counter) {
            UDS_SendNegativeResponse(reqData[0], 0x33); // Wrong Block Sequence Counter
            return;
        }

        uint8_t *data = &reqData[2];
        uint16_t dataLen = reqLen - 2;

        if(uds_active_session != UDS_SESSION_PROGRAMMING) {
        	UDS_SendNegativeResponse(reqData[0], 0x22); // CONDITIONS_NOT_CORRECT
        	return;
        }
        FlashManager_WritePartition(update_partition, download_address + bytes_received, data, dataLen);
        bytes_received += dataLen;
        block_counter++;

        uint8_t resp[1] = {blockNum};
        UDS_SendPositiveResponse(reqData[0], resp, 1);
    } else {
        UDS_SendNegativeResponse(reqData[0], 0x13);
    }
    return;
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
            {
            	UDS_HandleDiagnosticSessionControl(uds_rx_buffer, uds_rx_length);
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
        	UDS_HandleRequestDownload(uds_rx_buffer, uds_rx_length);
            break;

        case UDS_SID_TRANSFER_DATA:
        	UDS_HandleTransferData(uds_rx_buffer, uds_rx_length);
            break;

        case UDS_SID_REQUEST_TRANSFER_EXIT:
        	UDS_HandleTransferExit(uds_rx_buffer, uds_rx_length);
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

static void UDS_HandleTransferExit(uint8_t *reqData, uint16_t reqLen)
{
	update_done = 1;
	receivedCRC = (reqData[1] << 24) | (reqData[2] << 16) |
	                           (reqData[3] << 8) | (reqData[4]);

    if (bytes_received == download_size ) {
        UDS_SendPositiveResponse(reqData[0], NULL, 0);
    } else {
        UDS_SendNegativeResponse(reqData[0], 0x72); // General programming failure
    }
    return;
}

