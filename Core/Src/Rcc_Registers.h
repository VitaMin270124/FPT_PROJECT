#ifndef RCC_REGISTERS_H
#define RCC_REGISTERS_H

#include <stdint.h>

/** RCC Register Map (chỉ giữ phần cần cho CRC) */
typedef struct
{
    volatile uint32_t CR;        /**< Clock control register,              offset 0x00 */
    volatile uint32_t CFGR;      /**< Clock configuration register,        offset 0x04 */
    volatile uint32_t CIR;       /**< Clock interrupt register,            offset 0x08 */
    volatile uint32_t APB2RSTR;  /**< APB2 peripheral reset register,      offset 0x0C */
    volatile uint32_t APB1RSTR;  /**< APB1 peripheral reset register,      offset 0x10 */
    volatile uint32_t AHBENR;    /**< AHB peripheral clock enable register offset 0x14 */
    volatile uint32_t APB2ENR;   /**< APB2 peripheral clock enable register offset 0x18 */
    volatile uint32_t APB1ENR;   /**< APB1 peripheral clock enable register offset 0x1C */
    volatile uint32_t BDCR;      /**< Backup domain control register,      offset 0x20 */
    volatile uint32_t CSR;       /**< Control/status register,             offset 0x24 */
} RCC_TypeDef;

/** RCC base address */
#define RCC_BASE_ADDRESS (0x40021000u)

/** RCC pointer */
#define RCC ((RCC_TypeDef *)RCC_BASE_ADDRESS)

#endif
