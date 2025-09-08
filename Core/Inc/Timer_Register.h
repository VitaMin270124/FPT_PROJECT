#ifndef TIMER_REGISTER_H
#define TIMER_REGISTER_H

#include <stdint.h>

/* SysTick Registers Structure */
typedef struct {
    volatile uint32_t CSR;     /**< Control and Status Register */
    volatile uint32_t RVR;     /**< Reload Value Register */
    volatile uint32_t CVR;     /**< Current Value Register */
    volatile uint32_t CALIB;   /**< Calibration Register */
} SysTick_Type;

/* SysTick Base Address (Cortex-M3 standard) */
#define SYSTICK_BASE   (0xE000E010UL)
#define SYSTICK        ((SysTick_Type *)SYSTICK_BASE)

/* CSR Bit Definitions */
#define SYST_CSR_ENABLE_Pos      0
#define SYST_CSR_TICKINT_Pos     1
#define SYST_CSR_CLKSOURCE_Pos   2
#define SYST_CSR_COUNTFLAG_Pos   16

#define SYST_CSR_ENABLE      (1UL << SYST_CSR_ENABLE_Pos)
#define SYST_CSR_TICKINT     (1UL << SYST_CSR_TICKINT_Pos)
#define SYST_CSR_CLKSOURCE   (1UL << SYST_CSR_CLKSOURCE_Pos)
#define SYST_CSR_COUNTFLAG   (1UL << SYST_CSR_COUNTFLAG_Pos)

#endif /* TIMER_REGISTER_H */
