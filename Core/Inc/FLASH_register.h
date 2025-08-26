#ifndef FLASH_REGISTER_H
#define FLASH_REGISTER_H

/** FLASH - Register Layout Typedef */
typedef struct {
    volatile uint32_t ACR;       // 0x00: Flash access control register
    volatile uint32_t KEYR;      // 0x04: Flash key register
    volatile uint32_t OPTKEYR;   // 0x08: Flash option key register
    volatile uint32_t SR;        // 0x0C: Flash status register
    volatile uint32_t CR;        // 0x10: Flash control register
    volatile uint32_t AR;        // 0x14: Flash address register
    volatile uint32_t RESERVED;  // 0x18: Reserved
    volatile uint32_t OBR;       // 0x1C: Option byte register
    volatile uint32_t WRPR;      // 0x20: Write protection register
} FLASH_TypeDef_M;

// Base address
#define FLASH_BASE_M   (0x40022000UL)

// FLASH pointer
#define FLASH_M        ((FLASH_TypeDef_M *) FLASH_BASE_M)


#endif
