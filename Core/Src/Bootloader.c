#include "Bootloader.h"


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
    bool result = FlashManager_WritePartition(PARTITION_DATA_FLAGS,0,(const uint8_t*)flags, sizeof(Bootflag_t));
    FLASH_Lock();
    return result;
}

bool BL_VerifyFirmware(FlashPartitionId_t partId) {
    FlashPartitionInfo_t info;
    if (!FlashManager_GetPartitionInfo(partId, &info)) {
        return false;
    }
    current_flags.CRC = receivedCRC;
    uint32_t calculated_crc = BL_CalculateCRC(info.startAddress, info.size);
    if(calculated_crc = current_flags.CRC)
    	return true;
    else
    	return false;
}


bool BL_CheckForUpdateRequest(void) {

	GPIO_Clock_Enable(GPIOA);
	GPIO_Pin_Configure(GPIOA,10,GPIO_MODE_INPUT,GPIO_CNF_AFP_PP );
	if(GPIO_ReadPin(GPIOA,10) == 0)
	    return true;  // Nhấn nút → bootloader nhận firmware

    if (current_flags.update_firmware == UPDATE_REQUEST) {
    	current_flags.update_firmware = 0; // Xóa cờ sau khi đã đọc
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

void BL_Run(void) {
    CAN_Init(CAN_MODE_LOOPBACK ,500000);
    CANTP_Init();
    CAN_ITConfig(CAN1, CAN_IER_FMPIE0, ENABLE);
    // Đọc cờ trạng thái từ Flash
    FlashManager_ReadPartition(PARTITION_DATA_FLAGS, (uint8_t*)&current_flags, sizeof(Bootflag_t));

    // Check for update
    // Nếu có update vào UDS nhận và ghi vào flash theo sequence requestdownload -> transferdata -> transferexit
    if (BL_CheckForUpdateRequest()) {
    	UDS_Init();
    	current_flags.boot_flag = FLAG_MAIN_APP_VALID;
    	while(!update_done) {
    		UDS_MainFunction();
    	}
    }

    // Check CRC -> Jump app ....
    if (current_flags.boot_flag == FLAG_MAIN_APP_VALID) {
        if (BL_VerifyFirmware(PARTITION_APP_MAIN)) {
        	// Rollback_cpy .................
        	while(1)
        	{
        		if(Rollback_ToBackupApp()) break;
        	}
            BL_JumpToApplication(PARTITION_APP_MAIN);
        } else if (BL_VerifyFirmware(PARTITION_APP_BACKUP)) {
            // Cập nhật cờ
            current_flags.boot_flag = FLAG_BACKUP_APP_VALID;
            BL_UpdateFlags(&current_flags);
            BL_JumpToApplication(PARTITION_APP_BACKUP);
        }
    } else if (current_flags.boot_flag == FLAG_BACKUP_APP_VALID) {
            BL_JumpToApplication(PARTITION_APP_BACKUP);

    }
    while(1);
}
