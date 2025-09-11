/*
 * Bootloader.h
 *
 *  Created on: Sep 5, 2025
 *      Author: huynh
 */

#ifndef INC_BOOTLOADER_H_
#define INC_BOOTLOADER_H_

#include <stdint.h>
#include "FLASH_MANAGER.h"
#include "Bootloader_Flag.h"
#include "Crc.h"
#include "Clock.h"
#include "Crc_Registers.h"
#include "UDS.h"


typedef void (*pFunction)(void);

void BL_Run(void);
void BL_JumpToApplication(uint32_t app_address);
void BL_ReceiveAndFlashFirmware(void);
bool BL_CheckForUpdateRequest(void);
uint32_t BL_CalculateCRC(uint32_t start_address, uint32_t length);
bool BL_VerifyFirmware(FlashPartitionId_t partId);
bool BL_UpdateFlags(const Bootflag_t *flags);

#endif /* INC_BOOTLOADER_H_ */
