#include "gpio.h"
#include <stdint.h>


#define RCC_BASE            (0x40021000UL)
#define RCC_APB2ENR_OFFSET  (0x0C)
#define RCC_APB2ENR         *(volatile uint32_t *)(RCC_BASE + RCC_APB2ENR_OFFSET)

#define RCC_APB2ENR_IOPAEN_M (1UL << 2)
#define RCC_APB2ENR_IOPBEN_M (1UL << 3)
#define RCC_APB2ENR_IOPCEN_M (1UL << 4)

void GPIO_Clock_Enable(GPIO_TypeDef* GPIOx) {
    if (GPIOx == GPIOA) {
        RCC_APB2ENR |= RCC_APB2ENR_IOPAEN_M;
    } else if (GPIOx == GPIOB) {
        RCC_APB2ENR |= RCC_APB2ENR_IOPBEN_M;
    } else if (GPIOx == GPIOC) {
        RCC_APB2ENR |= RCC_APB2ENR_IOPCEN_M;
    }
}

void GPIO_Pin_Configure(GPIO_TypeDef* GPIOx, uint8_t pin, uint8_t mode, uint8_t cnf) {
    if (pin > 15) {
        return;
    }

    if (pin <= 7) {
        uint32_t shift = pin * 4;
        GPIOx->CRL &= ~(0xFUL << shift);
        GPIOx->CRL |= ((uint32_t)mode << shift) | ((uint32_t)cnf << (shift + 2));
    } else {
        pin -= 8;
        uint32_t shift = pin * 4;
        GPIOx->CRH &= ~(0xFUL << shift);
        GPIOx->CRH |= ((uint32_t)mode << shift) | ((uint32_t)cnf << (shift + 2));
    }
}

void GPIO_Pin_Set(GPIO_TypeDef* GPIOx, uint8_t pin) {
    GPIOx->BSRR = (1UL << pin);
}

void GPIO_Pin_Reset(GPIO_TypeDef* GPIOx, uint8_t pin) {
    GPIOx->BSRR = (1UL << (pin + 16));
}

void GPIO_Pin_Toggle(GPIO_TypeDef* GPIOx, uint8_t pin) {
    // Ð?c thanh ghi ODR d? ki?m tra tr?ng thái hi?n t?i c?a chân
    if ((GPIOx->ODR & (1UL << pin)) != 0) {
        GPIO_Pin_Reset(GPIOx, pin);
    } else {
        GPIO_Pin_Set(GPIOx, pin);
    }
}
