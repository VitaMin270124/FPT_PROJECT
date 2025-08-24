/**
  ******************************************************************************
  * @file    FLASH_module.h
  * @author  Minh_NG
  * @brief   Header file of Flash module.
  ******************************************************************************
  * Define to prevent recursive inclusion-------------------------------------*/
#ifndef __FLASH_MODULE_LL_H
#define __FLASH_MODULE_LL_H

/* Includes ------------------------------------------------------------------*/
#include "stdio.h"
#include "stdint.h"

/* FLASH_Private_Constants */
#define FLASH_KEY1_M   0x45670123U
#define FLASH_KEY2_M   0xCDEF89ABU


/* Exported types ------------------------------------------------------------*/
/**
  * @brief  FLASH register map
  */
typedef struct {
    volatile uint32_t ACR;       // 0x00: Flash access control register
    volatile uint32_t KEYR;      // 0x04: Flash key register
    volatile uint32_t OPTKEYR;   // 0x08: Flash option key register
    volatile uint32_t SR;        // 0x0C: Flash status register
    volatile uint32_t CR;        // 0x10: Flash control register
    volatile uint32_t AR;        // 0x14: Flash address register
    volatile uint32_t RESERVED;  // 0x18: Reserved
    volatile uint32_t OBR;       // 0x1C: Option byte register
    volatile uint32_t WRPR;      // 0x20: Write protection register
} FLASH_TypeDef_M;

// Base address
#define FLASH_BASE_M   (0x40022000UL)

// FLASH pointer
#define FLASH_M        ((FLASH_TypeDef_M *) FLASH_BASE_M)

/*
 * Example
 * Unlock flash
FLASH_M->KEYR = 0x45670123;
FLASH_M->KEYR = 0xCDEF89AB;
*/


/* Exported functions --------------------------------------------------------*/

/* IO operation functions *****************************************************/
uint16_t FLASH_ReadHalfWord(uint32_t address);
int FLASH_ProgramHalfWord(uint32_t address, uint16_t data);
int FLASH_ErasePage(uint32_t PageAddress);


/* Peripheral Control functions ***********************************************/
void FLASH_Unlock(void);
void FLASH_Lock(void);

#endif /* __FLASH_MODULE_LL_H */
