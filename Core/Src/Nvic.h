#ifndef NVIC_H
#define NVIC_H

#include "Nvic_Registers.h"

#define NVIC_MAX_INTERRUPT_NUMBER 60

typedef struct 
{
    uint8_t interruptNumber;  /**< Interrupt number to be configured */
    uint8_t priority;          /**< Priority level for the interrupt */
} Nvic_ConfigType;

/*!
 * @brief NVIC configuration structure
 *
 * This structure is used to configure the NVIC for a specific interrupt.
 *
 * @param[in] interruptNumber:  The interrupt number to be configured (IRQ number).
 * @param[in] priority:         The priority level for the interrupt (0 is highest priority).
 */

void NVIC_Init(const Nvic_ConfigType *config);
void NVIC_EnableInterrupt(uint8_t interruptNumber);
void NVIC_DisableInterrupt(uint8_t interruptNumber);
void NVIC_SetPending(uint8_t interruptNumber);
void NVIC_ClearPending(uint8_t interruptNumber);
uint8_t NVIC_GetActive(uint8_t interruptNumber);
void NVIC_SetPriority(uint8_t interruptNumber, uint8_t priority);
uint8_t NVIC_GetPriority(uint8_t interruptNumber);

#endif