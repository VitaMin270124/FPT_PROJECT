/*
 * FLASH_MANAGER.c
 *
 *  Created on: Sep 5, 2025
 *      Author: huynh
 */
/* Includes ------------------------------------------------------------------*/
#include "FLASH_MANAGER.h"
#include "FLASH_module.h"   // FLASH driver low level

/* Partition map */
static const FlashPartitionInfo_t partitionTable[PARTITION_MAX] = {
    { PARTITION_BOOTLOADER, 0x08000000, 12*1024, 1024 },   // 12 KB bootloader
    { PARTITION_APP_MAIN,   0x08004000, 24*1024, 1024 },   // 24 KB main app
    { PARTITION_APP_BACKUP, 0x08010000, 24*1024, 1024 },   // 24 KB backup app
    { PARTITION_DATA_FLAGS, 0x0801C000, 4*1024, 1024 }    // 4 KB flags/config
};

/* ===========================================================
 * Get partition info
 * =========================================================== */
bool FlashManager_GetPartitionInfo(FlashPartitionId_t partId, FlashPartitionInfo_t *info)
{
    if (partId >= PARTITION_MAX || info == NULL)
        return false;

    *info = partitionTable[partId];
    return true;
}

/* ===========================================================
 * Write data into partition (halfword programming)
 * =========================================================== */
bool FlashManager_WritePartition(FlashPartitionId_t partId,
                                 uint32_t offset,
                                 const uint8_t *data,
                                 uint32_t length)
{
    if (partId >= PARTITION_MAX || data == NULL)
        return false;

    FlashPartitionInfo_t part = partitionTable[partId];
    if ((offset + length) > part.size)
        return false;

    uint32_t writeAddr = part.startAddress + offset;

    FLASH_Unlock();

    uint32_t i = 0;
    while (i < length) {
        uint16_t halfword;

        if (i + 1 < length) {
            halfword = ((uint16_t)data[i+1] << 8) | data[i];
        } else {
            /* nếu length lẻ thì pad 0xFF */
            halfword = (0xFF << 8) | data[i];
        }

        if (FLASH_ProgramHalfWord(writeAddr, halfword) != 1) {
            FLASH_Lock();
            return false;
        }

        writeAddr += 2;
        i += 2;
    }

    FLASH_Lock();
    return true;
}


/* ===========================================================
 * READ data from partition
 * =========================================================== */
bool FlashManager_ReadPartition(FlashPartitionId_t partId, uint8_t *buffer, uint32_t length)
{
    if (partId >= PARTITION_MAX || buffer == NULL)
        return false;

    FlashPartitionInfo_t part = partitionTable[partId];
    if (length > part.size)
        return false;

    uint32_t addr = part.startAddress;
    uint32_t i = 0;
    while (i < length) {
        uint16_t halfword = FLASH_ReadHalfWord(addr);

        buffer[i] = (uint8_t)(halfword & 0xFF);
        if (i + 1 < length) {
            buffer[i+1] = (uint8_t)(halfword >> 8);
        }

        addr += 2;
        i += 2;
    }

    return true;
}


#include <stdint.h>
#include <stdbool.h>

bool FlashManager_ReadPartitionOffset(FlashPartitionId_t partId, uint8_t *buffer, uint32_t length, uint32_t offset)
{
    if (partId >= PARTITION_MAX || buffer == NULL)
        return false;

    FlashPartitionInfo_t part = partitionTable[partId];

    // Kiểm tra offset + length không vượt partition
    if (offset >= part.size || length > (part.size - offset))
        return false;

    uint32_t addr = part.startAddress + offset;
    uint32_t i = 0;

    while (i < length) {
        uint16_t halfword = FLASH_ReadHalfWord(addr);

        buffer[i] = (uint8_t)(halfword & 0xFF);
        if (i + 1 < length) {
            buffer[i+1] = (uint8_t)(halfword >> 8);
        }

        addr += 2;
        i += 2;
    }

    return true;
}


/* ===========================================================
 * Erase partition
 * =========================================================== */
bool FlashManager_ErasePartition(FlashPartitionId_t partId)
{
    if (partId >= PARTITION_MAX)
        return false;

    FlashPartitionInfo_t part = partitionTable[partId];
    uint32_t addr = part.startAddress;
    uint32_t endAddr = part.startAddress + part.size;

    FLASH_Unlock();
    while (addr < endAddr) {
        if (FLASH_ErasePage(addr) != 1) {
            FLASH_Lock();
            return false;
        }
        addr += part.pageSize;
    }
    FLASH_Lock();

    return true;
}

