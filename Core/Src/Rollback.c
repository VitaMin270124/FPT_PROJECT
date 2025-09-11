#include "Rollback.h"

#include <stdint.h>

// Rollback: copy toàn bộ dữ liệu từ Temp App (PARTITION_APP_BACKUP) sang Main App (PARTITION_APP_MAIN)
bool Rollback_ToMainApp(void)
{
    FlashPartitionInfo_t tempInfo, mainInfo;
    if (!FlashManager_GetPartitionInfo(PARTITION_APP_BACKUP, &tempInfo))
        return false;
    if (!FlashManager_GetPartitionInfo(PARTITION_APP_MAIN, &mainInfo))
        return false;

    // Buffer tạm để chứa dữ liệu temp app
    uint8_t buffer[24*1024]; // 24KB, đúng với kích thước vùng temp/main

    // Đọc dữ liệu từ Temp App
    if (!FlashManager_ReadPartition(PARTITION_APP_BACKUP, buffer, tempInfo.size))
        return false;

    // Xoá vùng Main App trước khi ghi
    if (!FlashManager_ErasePartition(PARTITION_APP_MAIN))
        return false;

    // Ghi dữ liệu từ buffer sang Main App
    if (!FlashManager_WritePartition(PARTITION_APP_MAIN, 0, buffer, mainInfo.size))
        return false;

    return true;
}
