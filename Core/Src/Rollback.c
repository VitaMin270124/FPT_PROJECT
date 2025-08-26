#include "Rollback.h"
#include "FLASH_MANAGER.h"
#include "Crc.h"

/**
 * @file    Rollback.c
 * @brief   Cơ chế Rollback/Commit firmware an toàn cho STM32F1.
 *
 * @details
 * File này hiện thực quy trình bảo vệ cập nhật firmware ở tầng Service.
 * Cơ chế gồm 4 thao tác chính:
 *  - Backup: sao lưu firmware đang chạy sang phân vùng BACKUP trước khi cập nhật.
 *  - Commit: xác minh (CRC32) firmware mới rồi ghi vào phân vùng MAIN nếu hợp lệ.
 *  - Restore: khôi phục lại firmware từ BACKUP khi phát hiện lỗi hoặc cập nhật thất bại.
 *  - Check: kiểm tra nhanh tính toàn vẹn firmware đang chạy để quyết định có cần rollback.
 *
 * Phụ thuộc:
 *  - FLASH_MANAGER.h  : dùng các API FlashManager_{GetPartitionInfo,ErasePartition,
 *                       ReadPartition,WritePartition} làm lớp trừu tượng thao tác flash.
 *  - Crc.h            : dùng CRC_Init/CRC_Reset/CRC_Calculate để tính CRC32 phần mềm.
 *
 * Cấu hình/giả định trong file:
 *  - FIRMWARE_SIZE, BACKUP_SIZE, EXPECTED_CRC_VALUE được định nghĩa tại file này
 *    (hoặc project config) cho phù hợp layout bộ nhớ thực tế.
 *  - Hai buffer `current_firmware_data[]` và `new_firmware_data[]` được
 *    `aligned(4)` và FIRMWARE_SIZE **phải là bội số của 4** vì CRC_Calculate
 *    nhận dữ liệu theo đơn vị word 32-bit (cast sang `const uint32_t*`).
 *  - PARTITION_APP_MAIN/PARTITION_APP_BACKUP đã ánh xạ đúng địa chỉ flash.
 *  - CRC_Init() nên được gọi một lần khi hệ thống khởi tạo (hoặc bảo vệ nội bộ).
 *
 * Trình tự khuyến nghị:
 *   1) Rollback_Backup_Firmware()
 *   2) Nạp dữ liệu FW mới vào `new_firmware_data` (hoặc từ vùng TEMP)
 *   3) Rollback_Commit_New_Firmware()  // CRC ok → ghi MAIN
 *   4) Nếu lỗi: Rollback_Restore_Firmware()
 *   5) Ở runtime có thể gọi Rollback_Check_Status() để giám sát tính toàn vẹn.
 *
 * Ghi chú:
 *  - Nên xử lý/ghi log lỗi trả về từ các hàm FlashManager_* (nếu có giá trị trả về).
 *  - EXPECTED_CRC_VALUE nên lấy từ metadata đi kèm gói FW (không hard-code khi lên sản phẩm).
 */
#define FIRMWARE_SIZE 1024 * 100  // 100KB firmware size
#define EXPECTED_CRC_VALUE 0xDEADBEEF  // Thay bằng giá trị CRC thực tế
#define BACKUP_SIZE FIRMWARE_SIZE  // Kích thước backup

/* Bảo đảm căn hàng 4 byte để cast an toàn */
__attribute__((aligned(4))) uint8_t current_firmware_data[FIRMWARE_SIZE];
__attribute__((aligned(4))) uint8_t new_firmware_data[FIRMWARE_SIZE];

void Rollback_Backup_Firmware(void) {
    FlashPartitionInfo_t backupPartition;

    // Get information about the backup partition
    if (FlashManager_GetPartitionInfo(PARTITION_APP_BACKUP, &backupPartition)) {
        // Erase the backup partition before writing
        FlashManager_ErasePartition(PARTITION_APP_BACKUP);

        // Write the current firmware to the backup partition
        FlashManager_WritePartition(PARTITION_APP_BACKUP, current_firmware_data, FIRMWARE_SIZE);
    }
}


void Rollback_Commit_New_Firmware(void)
{
    /* Khởi tạo CRC 1 lần ở nơi khởi tạo hệ thống, hoặc bảo vệ như sau:
       static int inited = 0; if(!inited){ CRC_Init(); inited = 1; } */

    /* cast sang uint32_t* và tính theo số WORDS */
    uint32_t calculated_crc =
        CRC_Calculate((const uint32_t*)new_firmware_data, FIRMWARE_SIZE / 4);

    if (calculated_crc == EXPECTED_CRC_VALUE) {
        FlashManager_WritePartition(PARTITION_APP_MAIN, new_firmware_data, FIRMWARE_SIZE);
    } else {
        /* TODO: xử lý lỗi */
    }
}


void Rollback_Restore_Firmware(void) {
    // Read the firmware from the backup partition
    uint8_t backup_firmware[BACKUP_SIZE];
    FlashManager_ReadPartition(PARTITION_APP_BACKUP, backup_firmware, BACKUP_SIZE);

    // Write the backup firmware to the main application partition
    FlashManager_WritePartition(PARTITION_APP_MAIN, backup_firmware, BACKUP_SIZE);
}


int Rollback_Check_Status(void)
{
    uint32_t current_crc =
        CRC_Calculate((const uint32_t*)current_firmware_data, FIRMWARE_SIZE / 4);

    return (current_crc != EXPECTED_CRC_VALUE);
}


