#ifndef CRC_H
#define CRC_H

#include "Crc_Registers.h"
#include <stddef.h>

/*!
 * @brief Initialize CRC module (if needed).
 * Enable CRC clock via RCC and reset calculation unit.
 */
void CRC_Init(void);

/*!
 * @brief Reset the CRC calculation state
 */
void CRC_Reset(void);

/*!
 * @brief Calculate CRC32 for a given data block.
 *
 * @param[in] data   Pointer to the input data buffer (32-bit words)
 * @param[in] length Number of 32-bit words in the data buffer
 * @return CRC32 value
 */
uint32_t CRC_Calculate(const uint32_t *data, size_t length);


#endif
