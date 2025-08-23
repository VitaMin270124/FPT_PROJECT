#ifndef NVIC_REGISTERS_H
#define NVIC_REGISTERS_H

#include <stdint.h>

/**  NVIC (Nested Vectored Interrupt Controller) Registers */

typedef struct 
{
    volatile uint32_t ISER[2];          /**< Interrupt Set-Enable Register */
    volatile uint32_t ICER[2];          /**< Interrupt Clear-Enable Register */
    volatile uint32_t ISPR[2];          /**< Interrupt Set-Pending Register */
    volatile uint32_t ICPR[2];          /**< Interrupt Clear-Pending Register */
    volatile const uint32_t IABR[2];    /**< Interrupt Active Bit Register */
    volatile uint8_t IPR[240];          /**< Interrupt Priority Register */
    
} NVIC_Typedef;

/** NVIC base address */
#define NVIC_BASE_ADDRESS 0xE000E100u

/** NVIC register structure pointer */
#define NVIC ((NVIC_Typedef *)NVIC_BASE_ADDRESS)


#endif 
