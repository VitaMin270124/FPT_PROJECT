#ifndef CLOCK_H
#define CLOCK_H

#include <Clock_Registers.h>

/**
 * @brief Initialize the system clock and configure necessary peripherals
 */
void clock_init(void);

/**
 * @brief Enable a specific peripheral clock.
 *
 * @param peripheral: The peripheral to enable.
 */
void clock_enable_peripheral(uint32_t peripheral);

/**
 * @brief Get the clock source of a peripheral.
 *
 * @param peripheral: The peripheral to query.
 * @return uint32_t: The clock source.
 */
uint32_t clock_get_peripheral(uint32_t peripheral);

/**
 * @brief Get the current AHB, APB1, APB2 or system bus clock
 *
 * @param bus_type: The bus type (AHB, APB1, APB2, SYS).
 * @return uint32_t: The current clock speed.
 */
uint32_t clock_get_bus(uint32_t bus_type);

/**
 * @brief Get the system clock speed.
 *
 * @return uint32_t: The system clock speed.
 */
uint32_t clock_get_system(void);

#endif
