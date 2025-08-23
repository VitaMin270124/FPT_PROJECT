#ifndef CRC_REGISTERS_H
#define CRC_REGISTERS_H

#include <stdint.h>

/** CRC Register Map */
typedef struct
{
    volatile uint32_t DR;       /**< Data Register, offset 0x00 */
    volatile uint8_t IDR;       /**< Input Data Register, offset 0x04 */
    uint8_t RESERVED0[3];       /**< IDR padding */
    volatile uint32_t CR;       /**< Control Register, offset 0x08 */
} CRC_typedef;

/** CRC Base Address */
#define CRC_BASE_ADDRESS ((CRC_typedef *)0x40023000u)

/** CRC Pointer */
#define CRC ((CRC_typedef *)CRC_BASE_ADDRESS)


#endif
