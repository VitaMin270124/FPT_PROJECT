/*
 * Bootloader_Flag.h
 *
 *  Created on: Sep 8, 2025
 *      Author: huynh
 */

#ifndef INC_BOOTLOADER_FLAG_H_
#define INC_BOOTLOADER_FLAG_H_

#define FLAG_MAIN_APP_VALID   0xAA
#define FLAG_BACKUP_APP_VALID 0xBB
#define FLAG_NO_VALID_APP     0xFF

#define UPDATE_REQUEST 0xDEADBEEFU

typedef struct{
	uint8_t update_firmware;
	uint8_t boot_flag;
	uint8_t main_flag;
	uint8_t rb_flag;
}Bootflag_t ;


#endif /* INC_BOOTLOADER_FLAG_H_ */
