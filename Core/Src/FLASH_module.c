/**
  ******************************************************************************
  * @file    FLASH_module.c
  * @author  Minh_NG
  * @brief   FLASH module driver.
  *          This file provides firmware functions to manage the following
  *          functionalities of the internal FLASH memory:
  *           + Program operations functions
  *           + Memory Control functions
  *
  ==============================================================================
                        ##### FLASH peripheral features #####
  ==============================================================================
  [..] The FLASH main features are:
      (+) Flash memory program/erase operations
      (+) Unlock/ Lock memory flash
      (+) Read halfword memory flash
  ==============================================================================
                     ##### How to use this driver #####
  ==============================================================================
  [..]
      This driver provides functions and macros to configure and program the FLASH
      memory of all STM32F1xx devices.

      (#) FLASH Memory I/O Programming functions: this group includes all needed
          functions to erase and program the main memory:
        (++) Lock and Unlock the FLASH interface
        (++) Erase function: Erase page, erase all pages
        (++) Program functions: half word
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include"FLASH_module.h"

/* Define ------------------------------------------------------------------*/
#define FLASH_CR_PG    (1U << 0)   // Programming bit
#define FLASH_SR_BSY   (1U << 0)   // Busy flag
#define FLASH_SR_EOP   (1U << 5)   // End of operation flag
#define FLASH_CR_PER   (1U << 1)   // Erasing bit
#define FLASH_CR_STRT  (1U << 6)   // Bit star to erase

/* Exported functions ---------------------------------------------------------*/
void FLASH_Unlock(void) {
	if(FLASH_M->CR & (1U<<7)) {  // bit 7 = Lock
		FLASH_M->KEYR = FLASH_KEY1_M;
		FLASH_M->KEYR = FLASH_KEY2_M;
	}
}

void FLASH_Lock(void) {
    FLASH_M->CR |= (1U << 7);   // Set LOCK bit
}

uint16_t FLASH_ReadHalfWord(uint32_t address) {
    return *(volatile uint16_t*)address;
}

/* Write 1 halfword (16-bit) into flash
 * return 1: Suscess
 * return -1: Fail
 */
int FLASH_ProgramHalfWord(uint32_t address, uint16_t data) {
    // 1. wait for Flash ready (BSY = 0)
    while (FLASH_M->SR & FLASH_SR_BSY);

    // 2. Set bit PG (enable programming)
    FLASH_M->CR |= FLASH_CR_PG;

    // 3. Write data (halfword) into address Flash
    *(volatile uint16_t*)address = data;

    // 4. Wait for writing (BSY = 0)
    while (FLASH_M->SR & FLASH_SR_BSY);

    // 5. Clear bit PG
    FLASH_M->CR &= ~FLASH_CR_PG;

    // 6. Check EOP (End of Programming)
    if (FLASH_M->SR & FLASH_SR_EOP) {
        FLASH_M->SR = FLASH_SR_EOP; // Write 1 to clear flag
        return 1; // Success
    }

    return -1; // Fail
}

/* Erase 1 page (1KB)
 * return 1: Suscess
 * return -1: Fail
 */
int FLASH_ErasePage(uint32_t PageAddress) {
    // 1. Waiting flash if flash busy
    while (FLASH_M->SR & FLASH_SR_BSY);

    // 2. Set bit PER (enable erasing)
    FLASH_M->CR |= FLASH_CR_PER;

    // 3. Select address page to erase
    FLASH_M->AR = PageAddress;

    // 4. Begin erase
    FLASH_M->CR |= FLASH_CR_STRT;

    // 5. Waiting for erasing
    while (FLASH_M->SR & FLASH_SR_BSY);

    // 6. Clear bit PER
    FLASH_M->CR &= ~FLASH_CR_PER;

    // 7. Check EOP (End of Programming)
    if (FLASH_M->SR & FLASH_SR_EOP) {
    	FLASH_M->SR = FLASH_SR_EOP; // Write 1 to clear flag
        	return 1; // Success
    }
    return -1; // Fail
}

