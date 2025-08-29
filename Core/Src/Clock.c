#include "clock.h"

/* Function to initialize the system clock */
void clock_init(void) {
    /* Enable HSI (High-Speed Internal) */
    RCC->CR |= RCC_CR_HSION;

    /* Wait for HSI to be ready */
    while (!(RCC->CR & RCC_CR_HSION));

    /* Select HSI as system clock source */
    RCC->CFGR &= ~RCC_CFGR_SW;
    RCC->CFGR |= (0x0 << RCC_CFGR_SW);

    /* Enable PLL (Phase-Locked Loop) */
    RCC->CR |= RCC_CR_PLLON;
    while (!(RCC->CR & RCC_CR_PLLON));
}

/* Function to enable a peripheral clock */
void clock_enable_peripheral(uint32_t peripheral) {
    if (peripheral == RCC_APB1RSTR_TIM2RST) {
        RCC->APB1RSTR |= RCC_APB1RSTR_TIM2RST;
    } else if (peripheral == RCC_APB2RSTR_USART1RST) {
        RCC->APB2RSTR |= RCC_APB2RSTR_USART1RST;
    }
}

/* Function to get the clock source of a specific peripheral */
uint32_t clock_get_peripheral(uint32_t peripheral) {
    if (peripheral == RCC_APB1RSTR_TIM2RST) {
        return (RCC->CFGR & RCC_CFGR_SWS);
    } else if (peripheral == RCC_APB2RSTR_USART1RST) {
        return (RCC->CFGR & RCC_CFGR_SWS);
    }
    return 0;
}

/* Function to get the current clock speed of a specified bus */
uint32_t clock_get_bus(uint32_t bus_type) {
    switch (bus_type) {
        case 0: // AHB
            return (RCC->CFGR & RCC_CFGR_HPRE);
        case 1: // APB1
            return (RCC->CFGR & RCC_CFGR_SWS);
        case 2: // APB2
            return (RCC->CFGR & RCC_CFGR_SWS);
        case 3: // SYS
            return clock_get_system();
        default:
            return 0;
    }
}

/* Function to get the system clock speed */
uint32_t clock_get_system(void) {
    return (RCC->CFGR & RCC_CFGR_SWS);
}