#ifndef ROLLBACK_H
#define ROLLBACK_H

#include "FLASH_MANAGER.h"  // Quản lý bộ nhớ flash
#include "Crc.h"             // Kiểm tra CRC của firmware

// Khai báo các hàm cho phần rollback
void Rollback_Backup_Firmware(void);
void Rollback_Commit_New_Firmware(void);
void Rollback_Restore_Firmware(void);
int Rollback_Check_Status(void);

// Khai báo các biến toàn cục để lưu trữ dữ liệu firmware hiện tại và mới
extern uint8_t current_firmware_data[];  // Dữ liệu firmware hiện tại
extern uint8_t new_firmware_data[];      // Dữ liệu firmware mới
extern uint32_t FIRMWARE_SIZE;           // Kích thước firmware
extern uint32_t EXPECTED_CRC_VALUE;     // Giá trị CRC mong đợi
extern uint32_t BACKUP_SIZE;            // Kích thước backup

#endif /* ROLLBACK_H */
