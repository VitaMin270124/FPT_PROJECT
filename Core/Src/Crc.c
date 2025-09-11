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

uint32_t CRC_Calculate(const void *data, size_t length) {
    if (length == 0) return 0;

    CRC_Init();

    const uint8_t *byte_ptr = (const uint8_t *)data;

    uint32_t word_count = length / 4;
    for (uint32_t i = 0; i < word_count; i++) {
        CRC->DR = ((const uint32_t *)byte_ptr)[i];
    }

    uint32_t remainder_bytes = length % 4;
    if (remainder_bytes > 0) {
        uint32_t last_word = 0;
        memcpy(&last_word, byte_ptr + (word_count * 4), remainder_bytes);
        CRC->DR = last_word;
    }

    return CRC->DR;
}
