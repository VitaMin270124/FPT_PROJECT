/**
  ******************************************************************************
  * @file    FLASH_MANAGER.c
  * @author  Minh_NG
  * @brief   FLASH MANAGER service.
  *          This file provides service functions to manage Flash partitions
  *          on top of the low-level FLASH driver. It abstracts raw memory
  *          operations into application-oriented APIs.
  *
  ==============================================================================
                        ##### FLASH MANAGER features #####
  ==============================================================================
  [..] The FLASH MANAGER main features are:
      (+) Partition abstraction layer for Flash memory
      (+) High-level read/write/erase APIs by partition ID
      (+) Automatic erase before write
      (+) Support for halfword-based programming (uses low-level driver)
      (+) Data integrity handling (optional verify after write)
  ==============================================================================
                     ##### How to use this service #####
  ==============================================================================
  [..]
      This service provides functions and data structures to manage Flash
      memory partitions in STM32F1xx devices.

      (#) FLASH Partition concept:
          Each partition is defined with:
            (++) Start address
            (++) Size
            (++) Page size (erase granularity)

      (#) FLASH MANAGER I/O functions:
          (++) FlashManager_GetPartitionInfo() : Retrieve partition info
          (++) FlashManager_ReadPartition()    : Read data from partition
          (++) FlashManager_WritePartition()   : Erase + Program data into partition
          (++) FlashManager_ErasePartition()   : Erase an entire partition

      (#) Typical usage example:
          (++) Define partitions in flash_manager.c (Bootloader, App, Backup, Data)
          (++) Use FlashManager_WritePartition() to program firmware or config
          (++) Use FlashManager_ReadPartition() to retrieve stored data
          (++) Use FlashManager_ErasePartition() before re-programming
  ******************************************************************************
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
bool FlashManager_WritePartition(FlashPartitionId_t partId, const uint8_t *data, uint32_t length)
{
    if (partId >= PARTITION_MAX || data == NULL)
        return false;

    FlashPartitionInfo_t part = partitionTable[partId];
    if (length > part.size)
        return false;

    /* Erase partition before writting */
    uint32_t addr = part.startAddress;
    uint32_t endAddr = part.startAddress + part.size;

    FLASH_Unlock();
    while (addr < endAddr) {
        if (FLASH_ErasePage(addr) != 1) { // different 1 is fail
            FLASH_Lock();
            return false;
        }
        addr += part.pageSize;
    }

    /* Write data halfword at a time (16-bit) */
    uint32_t writeAddr = part.startAddress;
    uint32_t i = 0;
    while (i < length) {
        uint16_t halfword = 0xFFFF;

        if (i + 1 < length) {
            halfword = ((uint16_t)data[i+1] << 8) | data[i];
        } else {
            /* if byte is odd, pad 0xFF */
            halfword = (0xFF << 8) | data[i];
        }

        if (FLASH_ProgramHalfWord(writeAddr, halfword) != 0) {
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
        if (FLASH_ErasePage(addr) != 0) {
            FLASH_Lock();
            return false;
        }
        addr += part.pageSize;
    }
    FLASH_Lock();

    return true;
}
