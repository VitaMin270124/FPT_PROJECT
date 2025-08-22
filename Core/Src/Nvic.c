#include "Nvic.h"

/*!
 * @brief NVIC configuration structure
 *
 * This structure is used to configure the NVIC for a specific interrupt.
 *
 * @param[in] interruptNumber:  The interrupt number to be configured (IRQ number).
 * @param[in] priority:         The priority level for the interrupt (0 is highest priority).
 */

void NVIC_Init(const Nvic_ConfigType *config)
{
    NVIC_SetPriority(config->interruptNumber, config->priority);
    NVIC_EnableInterrupt(config->interruptNumber);
}

 void NVIC_EnableInterrupt(uint8_t interruptNumber)
{
    if (interruptNumber < NVIC_MAX_INTERRUPT_NUMBER) 
    {
        NVIC->ISER[interruptNumber / 32] |= (1U << (interruptNumber % 32));
    }
}

void NVIC_DisableInterrupt(uint8_t interruptNumber)
{
    if (interruptNumber < NVIC_MAX_INTERRUPT_NUMBER) 
    {
        NVIC->ICER[interruptNumber / 32] |= (1U << (interruptNumber % 32));
    }
}

void NVIC_SetPending(uint8_t interruptNumber)
{
    if (interruptNumber < NVIC_MAX_INTERRUPT_NUMBER) 
    {
        NVIC->ISPR[interruptNumber / 32] |= (1U << (interruptNumber % 32));
    }
}

void NVIC_ClearPending(uint8_t interruptNumber)
{
    if (interruptNumber < NVIC_MAX_INTERRUPT_NUMBER) 
    {
        NVIC->ICPR[interruptNumber / 32] |= (1U << (interruptNumber % 32));
    }
}

uint8_t NVIC_GetActive(uint8_t interruptNumber)
{
    if (interruptNumber < NVIC_MAX_INTERRUPT_NUMBER) 
    {
        return ((NVIC->IABR[interruptNumber / 32] >> (interruptNumber % 32)) & 0x01u);
    }
    return 0;
}

void NVIC_SetPriority(uint8_t interruptNumber, uint8_t priority)
{
    if (interruptNumber < NVIC_MAX_INTERRUPT_NUMBER)
    {
        NVIC->IPR[interruptNumber] = (priority & 0x0Fu) << 4; // Assuming 4-bit (high) priority
    }
}

uint8_t NVIC_GetPriority(uint8_t interruptNumber)
{
    if (interruptNumber < NVIC_MAX_INTERRUPT_NUMBER)
    {
        return ((NVIC->IPR[interruptNumber] >> 4) & 0x0Fu); // Assuming 4-bit (high) priority
    }
    return 0;
}
