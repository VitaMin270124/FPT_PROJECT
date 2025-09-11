/*
 * Bootloader.h
 *
 *  Created on: Sep 5, 2025
 *      Author: huynh
 */

#ifndef INC_BOOTLOADER_H_
#define INC_BOOTLOADER_H_

#include <stdint.h>
#include "FLASH_MANAGER.h"
#include "Bootloader_Flag.h"
#include "Crc.h"
#include "Clock.h"
#include "Crc_Registers.h"
#include "UDS.h"
#include "gpio.h"
#include "can.h"
#include "Rollback.h"

typedef struct {
  volatile uint32_t CPUID;                  /*!< Offset: 0x000 (R/ )  CPUID Base Register */
  volatile uint32_t ICSR;                   /*!< Offset: 0x004 (R/W)  Interrupt Control and State Register */
  volatile uint32_t VTOR;                   /*!< Offset: 0x008 (R/W)  Vector Table Offset Register */
  volatile uint32_t AIRCR;                  /*!< Offset: 0x00C (R/W)  Application Interrupt and Reset Control Register */
  volatile uint32_t SCR;                    /*!< Offset: 0x010 (R/W)  System Control Register */
  volatile uint32_t CCR;                    /*!< Offset: 0x014 (R/W)  Configuration Control Register */
  volatile uint32_t SHPR[3];                /*!< Offset: 0x018 (R/W)  System Handlers Priority Registers (4-7, 8-11, 12-15) */
  volatile uint32_t SHCSR;                  /*!< Offset: 0x024 (R/W)  System Handler Control and State Register */
  volatile uint32_t CFSR;                   /*!< Offset: 0x028 (R/W)  Configurable Fault Status Register */
  volatile uint32_t HFSR;                   /*!< Offset: 0x02C (R/W)  HardFault Status Register */
  volatile uint32_t DFSR;                   /*!< Offset: 0x030 (R/W)  Debug Fault Status Register */
  volatile uint32_t MMFAR;                  /*!< Offset: 0x034 (R/W)  MemManage Fault Address Register */
  volatile uint32_t BFAR;                   /*!< Offset: 0x038 (R/W)  BusFault Address Register */
  volatile uint32_t AFSR;                   /*!< Offset: 0x03C (R/W)  Auxiliary Fault Status Register */
  volatile uint32_t PFR[2];                 /*!< Offset: 0x040 (R/ )  Processor Feature Register */
  volatile uint32_t DFR;                    /*!< Offset: 0x048 (R/ )  Debug Feature Register */
  volatile uint32_t ADR;                    /*!< Offset: 0x04C (R/ )  Auxiliary Feature Register */
  volatile uint32_t MMFR[4];                /*!< Offset: 0x050 (R/ )  Memory Model Feature Register */
  volatile uint32_t ISAR[5];                /*!< Offset: 0x060 (R/ )  ISA Feature Register */
} SCB_Type;

// Định nghĩa con trỏ đến SCB, trỏ tới địa chỉ cơ sở của SCB trong không gian bộ nhớ.
// Địa chỉ này là cố định cho tất cả các lõi Cortex-M3 (0xE000ED00).
#define SCB_BASE            (0xE000ED00UL)                            /*!< System Control Block Base Address */
#define SCB                 ((SCB_Type *) SCB_BASE)                   /*!< System Control Block */

// Các hàm nội tại (intrinsic functions) cho barrierrs và set MSP
// Nếu trình biên dịch của bạn (ví dụ: GCC) hỗ trợ, bạn có thể dùng các hàm này.
#ifndef __DSB
#define __DSB() __asm volatile ("dsb sy" ::: "memory")
#endif
#ifndef __ISB
#define __ISB() __asm volatile ("isb sy" ::: "memory")
#endif
#ifndef __set_MSP
#define __set_MSP(msp) __asm volatile ("msr msp, %0\n" : : "r" (msp) : "sp")
#endif
#ifndef __disable_irq
#define __disable_irq() __asm volatile ("cpsid i" : : : "memory")
#endif
#ifndef __enable_irq
#define __enable_irq() __asm volatile ("cpsie i" : : : "memory")
#endif


typedef void (*pFunction)(void);

void BL_Run(void);
void BL_JumpToApplication(uint32_t app_address);
void BL_ReceiveAndFlashFirmware(void);
bool BL_CheckForUpdateRequest(void);
uint32_t BL_CalculateCRC(uint32_t start_address, uint32_t length);
bool BL_VerifyFirmware(FlashPartitionId_t partId);
bool BL_UpdateFlags(const Bootflag_t *flags);

#endif /* INC_BOOTLOADER_H_ */
