#ifndef CLOCK_REGISTERS_H
#define CLOCK_REGISTERS_H

#include <stdint.h>

/** RCC Register Map */
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

/* RCC_CR Bits */
#define RCC_CR_HSION   (1 << 0)      /* HSI On */
#define RCC_CR_HSEON   (1 << 16)     /* HSE On */
#define RCC_CR_PLLON   (1 << 24)     /* PLL On */

/* RCC_CFGR Bits */
#define RCC_CFGR_SW    (3 << 0)      /* System Clock Switch */
#define RCC_CFGR_SWS   (3 << 2)      /* System Clock Switch Status */
#define RCC_CFGR_HPRE  (15 << 4)     /* AHB Prescaler */

/* RCC_APB1RSTR and RCC_APB2RSTR */
#define RCC_APB1RSTR_TIM2RST    (1 << 0)    /* Timer 2 Reset */
#define RCC_APB2RSTR_USART1RST  (1 << 14)   /* USART1 Reset */

/* RCC_BDCR Bits */
#define RCC_BDCR_LSEON   (1 << 0)      /* LSE On */
#define RCC_BDCR_RTCEN   (1 << 15)     /* RTC Enable */

/* RCC_CSR Bits */
#define RCC_CSR_LSION    (1 << 0)      /* LSI On */

#endif
