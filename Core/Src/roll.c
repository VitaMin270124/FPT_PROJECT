#include "Rollback.h"
#include <stdint.h>
#include <stdbool.h>

#define ROLLBACK_BLOCK_SIZE 1024  // 1 KB block

// Rollback: copy toàn bộ dữ liệu từ Main App sang Backup App
bool Rollback_ToBackupApp(void)
{
    FlashPartitionInfo_t mainInfo, backupInfo;

    // Lấy thông tin partition
    if (!FlashManager_GetPartitionInfo(PARTITION_APP_MAIN, &mainInfo))
        return false;
    if (!FlashManager_GetPartitionInfo(PARTITION_APP_BACKUP, &backupInfo))
        return false;

    // Xoá vùng Backup App trước khi ghi
    if (!FlashManager_ErasePartition(PARTITION_APP_BACKUP))
        return false;

    // Buffer tạm để copy theo block
    uint8_t buffer[ROLLBACK_BLOCK_SIZE];

    uint32_t offset = 0;
    while (offset < mainInfo.size)
    {
        uint32_t blockSize = (mainInfo.size - offset > ROLLBACK_BLOCK_SIZE) ? ROLLBACK_BLOCK_SIZE : (mainInfo.size - offset);

        // Đọc dữ liệu từ Main App
        if (!FlashManager_ReadPartitionOffset(PARTITION_APP_MAIN, buffer, blockSize, offset))
            return false;

        // Ghi dữ liệu sang Backup App
        if (!FlashManager_WritePartition(PARTITION_APP_BACKUP, offset, buffer, blockSize))
            return false;

        offset += blockSize;
    }

    return true;
}
