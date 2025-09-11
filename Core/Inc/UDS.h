#ifndef UDS_H
#define UDS_H

#include <stdint.h>
#include <stdbool.h>
#include "cantp.h"
#include "FLASH_MANAGER.h"

// UDS Service IDs (ISO 14229-1)
#define UDS_SID_DIAGNOSTIC_SESSION_CONTROL   0x10
#define UDS_SID_ECU_RESET                    0x11
#define UDS_SID_CLEAR_DIAGNOSTIC_INFORMATION 0x14
#define UDS_SID_READ_DATA_BY_IDENTIFIER      0x22
#define UDS_SID_SECURITY_ACCESS              0x27
#define UDS_SID_COMMUNICATION_CONTROL        0x28
#define UDS_SID_TESTER_PRESENT               0x3E
#define UDS_SID_ACCESS_TIMING_PARAMETER      0x83
#define UDS_SID_REQUEST_DOWNLOAD             0x34
#define UDS_SID_TRANSFER_DATA                0x36
#define UDS_SID_REQUEST_TRANSFER_EXIT        0x37

// Negative Response Code (NRC)
#define UDS_NRC_GENERAL_REJECT               0x10
#define UDS_NRC_SERVICE_NOT_SUPPORTED        0x11
#define UDS_NRC_SUBFUNCTION_NOT_SUPPORTED    0x12
#define UDS_NRC_INCORRECT_MESSAGE_LENGTH     0x13
#define UDS_NRC_BUSY_REPEAT_REQUEST          0x21
#define UDS_NRC_CONDITIONS_NOT_CORRECT       0x22
#define UDS_NRC_REQUEST_SEQUENCE_ERROR       0x24
#define UDS_NRC_REQUEST_OUT_OF_RANGE         0x31
#define UDS_NRC_SECURITY_ACCESS_DENIED       0x33

// Typedef
// Session Type
typedef enum {
    UDS_SESSION_DEFAULT     = 0x01,
    UDS_SESSION_PROGRAMMING = 0x02,
    UDS_SESSION_EXTENDED    = 0x03
} UDS_SessionType;

// API prototypes
void UDS_Init();
UDS_SessionType UDS_GetCurrentSession(void);   // ------------ NEW  --------------- //
void UDS_MainFunction(void);
void UDS_RxIndication(uint32_t can_id, uint8_t *data, uint8_t dlc);
void UDS_SendPositiveResponse(uint8_t sid, const uint8_t *data, uint16_t len);
void UDS_SendNegativeResponse(uint8_t sid, uint8_t nrc);

#endif // UDS_H
