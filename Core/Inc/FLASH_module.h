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
#include "FLASH_register.h"

/* FLASH_Private_Constants */
#define FLASH_KEY1_M   0x45670123U
#define FLASH_KEY2_M   0xCDEF89ABU


/* Exported types ------------------------------------------------------------*/

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
