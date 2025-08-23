#ifndef CLOCK_REGISTERS_H
#define CLOCK_REGISTERS_H

#include "stdint.h"

/* RCC (Reset and Clock Control) Register Definitions */
#define RCC_CR         (*(volatile uint32_t *)0x40021000)    /* RCC Control Register */
#define RCC_CFGR       (*(volatile uint32_t *)0x40021004)    /* RCC Clock Configuration Register */
#define RCC_APB1RSTR   (*(volatile uint32_t *)0x4002101C)    /* RCC APB1 Reset Register */
#define RCC_APB2RSTR   (*(volatile uint32_t *)0x40021018)    /* RCC APB2 Reset Register */
#define RCC_BDCR       (*(volatile uint32_t *)0x40021020)    /* RCC Backup Domain Control Register */
#define RCC_CSR        (*(volatile uint32_t *)0x40021024)    /* RCC Control/Status Register */

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
