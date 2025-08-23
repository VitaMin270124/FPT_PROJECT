#ifndef CLOCK_H
#define CLOCK_H

/**
 * @brief Function to initialize system clock using HSI as the clock source.
 *
 * This function configures the system to use High-Speed Internal (HSI) as the clock source,
 * and sets up the PLL to enhance the system clock.
 */
void Clock_Init(void);

#endif // CLOCK_H
