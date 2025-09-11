


static Bootflag_t current_flags;
static uint32_t firmware_received_count = 0;

uint32_t BL_CalculateCRC(uint32_t start_address, uint32_t length) {
    // Bootloader chỉ wrap lại cho tiện dụng
    return CRC_Calculate((const void *)start_address, length);
}

bool BL_UpdateFlags(const Bootflag_t *flags) {
    FLASH_Unlock();
    if (!FlashManager_ErasePartition(PARTITION_DATA_FLAGS)) {
        FLASH_Lock();
        return false;
    }
    bool result = FlashManager_WritePartition(PARTITION_DATA_FLAGS, (const uint8_t*)flags, sizeof(Bootflag_t));
    FLASH_Lock();
    return result;
}

bool BL_VerifyFirmware(FlashPartitionId_t partId) {
    FlashPartitionInfo_t info;
    if (!FlashManager_GetPartitionInfo(partId, &info)) {
        return false;
    }
    uint32_t calculated_crc = BL_CalculateCRC(info.startAddress, info.size);
    if (partId == PARTITION_APP_MAIN) {
        return (calculated_crc == current_flags.main_flag) && (current_flags.main_flag != 0xFFFFFFFF);
    } else if (partId == PARTITION_APP_BACKUP) {
        return (calculated_crc == current_flags.rb_flag) && (current_flags.rb_flag != 0xFFFFFFFF);
    }
    return false;
}


bool BL_CheckForUpdateRequest(void) {

	if(GPIO_ReadPin(BOOT_PIN) == PIN_SET)
	        return true;  // Nhấn nút → bootloader nhận firmware

    if (current_flags.update_firmware == UPDATE_REQUEST) {
        Bootflag_t temp_flags = current_flags;
        temp_flags.update_firmware = 0; // Xóa cờ sau khi đã đọc
        BL_UpdateFlags(&temp_flags);
        return true;
    }
    	return false;
}

void BL_JumpToApplication(uint32_t app_address) {
    if (*(volatile uint32_t*)app_address != 0xFFFFFFFF) {
       __disable_irq();
       // CAN_DeInit(CAN1);
        uint32_t app_stack_pointer = *(volatile uint32_t*)app_address;
        uint32_t app_reset_handler_address = *(volatile uint32_t*)(app_address + 4);
        pFunction app_reset_handler = (pFunction)app_reset_handler_address;

        // Set lại vector table để ngắt trỏ đúng về App
        SCB->VTOR = app_address;
        __DSB();
        __ISB();

        // Set MSP cho Application
        __set_MSP(app_stack_pointer);

        __enable_irq();

        app_reset_handler();
    }
}

//void BL_ReceiveAndFlashFirmware(void) {
//    uint8_t rx_data_buffer[256];
//    uint16_t received_length;
//    firmware_received_count = 0;
//
//    FlashPartitionInfo_t main_info;
//    FlashManager_GetPartitionInfo(PARTITION_APP_MAIN, &main_info);
//
//
//
//    FlashManager_ErasePartition(PARTITION_APP_MAIN);
//
//    while (1) {
//        CANTP_MainFunction();
//        received_length = CANTP_ReadRxData(rx_data_buffer, sizeof(rx_data_buffer));
//
//        if (received_length > 0) {
//            if (FlashManager_WritePartition(PARTITION_APP_MAIN, rx_data_buffer, received_length) == false) {
//                 break;
//            }
//            firmware_received_count += received_length;
//            if (firmware_received_count >= main_info.size) {
//                break;
//            }
//        }
//    }
//
//    // Sau khi nạp xong, cập nhật CRC và cờ
//    current_flags.main_flag = BL_CalculateCRC(main_info.startAddress, main_info.size);
//    current_flags.boot_flag = FLAG_MAIN_APP_VALID;
//    BL_UpdateFlags(&current_flags);
//}

void BL_Run(void) {
    CAN_Init(CAN_MODE_LOOPBACK,9600);
    CANTP_Init();

    // Đọc cờ trạng thái từ Flash
    FlashManager_ReadPartition(PARTITION_DATA_FLAGS, (uint8_t*)&current_flags, sizeof(Bootflag_t));

    if (BL_CheckForUpdateRequest()) {
    	UDS_Init();
    	while(!update_done) {
    		UDS_MainFunction();
    	}
    	// Check CRC ....
    }

    if (current_flags.boot_flag == FLAG_MAIN_APP_VALID) {
        if (BL_VerifyFirmware(PARTITION_APP_MAIN)) {
            BL_JumpToApplication(PARTITION_APP_MAIN);
        } else if (BL_VerifyFirmware(PARTITION_APP_BACKUP)) {
            BL_JumpToApplication(PARTITION_APP_BACKUP);
            // Cập nhật cờ
            current_flags.boot_flag = FLAG_BACKUP_APP_VALID;
            BL_UpdateFlags(&current_flags);
        }
    } else if (current_flags.boot_flag == FLAG_BACKUP_APP_VALID) {
        if (BL_VerifyFirmware(PARTITION_APP_BACKUP)) {
            BL_JumpToApplication(PARTITION_APP_BACKUP);
        } else if (BL_VerifyFirmware(PARTITION_APP_MAIN)) {
            BL_JumpToApplication(PARTITION_APP_MAIN);
            // Cập nhật cờ
            current_flags.boot_flag = FLAG_MAIN_APP_VALID;
            BL_UpdateFlags(&current_flags);
        }
    }
    while(1);
}
