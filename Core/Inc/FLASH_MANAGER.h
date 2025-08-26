/**
  ******************************************************************************
  * @file    FLASH_MANAGER.h
  * @author  Minh_NG
  * @brief   Header file of FLASH MANAGER.
  ******************************************************************************
  * Define to prevent recursive inclusion-------------------------------------*/
#ifndef FLASH_MANAGER_H
#define FLASH_MANAGER_H

#include <stdint.h>
#include <stdbool.h>

typedef enum {
    PARTITION_BOOTLOADER = 0,
    PARTITION_APP_MAIN,
    PARTITION_APP_BACKUP,
    PARTITION_DATA_FLAGS,
    PARTITION_MAX
} FlashPartitionId_t;

typedef struct {
    FlashPartitionId_t id;
    uint32_t startAddress;
    uint32_t size;
    uint32_t pageSize;   /* kích thước 1 page */
} FlashPartitionInfo_t;

/* Public APIs */
bool FlashManager_GetPartitionInfo(FlashPartitionId_t partId, FlashPartitionInfo_t *info);
bool FlashManager_WritePartition(FlashPartitionId_t partId, const uint8_t *data, uint32_t length);
bool FlashManager_ReadPartition(FlashPartitionId_t partId, uint8_t *buffer, uint32_t length);
bool FlashManager_ErasePartition(FlashPartitionId_t partId);

#endif /* FLASH_MANAGER_H */
