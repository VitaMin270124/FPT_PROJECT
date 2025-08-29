#include "Crc.h"
#include "Clock_Registers.h"


void CRC_Init(void) {
    /* Enable CRC clock in RCC (AHBENR bit 6 = CRCEN) */
    RCC->AHBENR |= (1U << 6U);
    
    

    /* Reset CRC calculation unit */
    CRC_Reset();
}

void CRC_Reset(void) {
    CRC->CR = 1u;
}

uint32_t CRC_Calculate(const uint32_t *data, size_t length) {
    /* Reset CRC before new calculation */
    CRC_Reset();

    for( size_t i = 0; i < length; i++) 
    {
        CRC->DR = data[i];
    }

    /* Return the computed CRC value */
    return CRC->DR;
}
