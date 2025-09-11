#ifndef __GPIO_H
#define __GPIO_H

#include <stdint.h>

#define GPIO_MODE_INPUT       0x00
#define GPIO_MODE_OUTPUT_10MHZ  0x01
#define GPIO_MODE_OUTPUT_2MHZ   0x02
#define GPIO_MODE_OUTPUT_50MHZ  0x03

#define GPIO_CNF_GPP_PP      0x00 
#define GPIO_CNF_GPP_OD      0x01 
#define GPIO_CNF_AFP_PP      0x02 
#define GPIO_CNF_AFP_OD      0x03 


typedef struct {
    volatile uint32_t CRL;
    volatile uint32_t CRH;
    volatile uint32_t IDR;
    volatile uint32_t ODR;
    volatile uint32_t BSRR;
    volatile uint32_t BRR;
    volatile uint32_t LCKR;
} GPIO_TypeDef;


#define GPIOA_BASE            (0x40010800UL)
#define GPIOB_BASE            (0x40010C00UL)
#define GPIOC_BASE            (0x40011000UL)

#define GPIOA                 ((GPIO_TypeDef *) GPIOA_BASE)
#define GPIOB                 ((GPIO_TypeDef *) GPIOB_BASE)
#define GPIOC                 ((GPIO_TypeDef *) GPIOC_BASE)


void GPIO_Clock_Enable(GPIO_TypeDef* GPIOx);
void GPIO_Pin_Configure(GPIO_TypeDef* GPIOx, uint8_t pin, uint8_t mode, uint8_t cnf);
void GPIO_Pin_Set(GPIO_TypeDef* GPIOx, uint8_t pin);
void GPIO_Pin_Reset(GPIO_TypeDef* GPIOx, uint8_t pin);
void GPIO_Pin_Toggle(GPIO_TypeDef* GPIOx, uint8_t pin);

#endif // __GPIO_H