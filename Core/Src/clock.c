#include "stm32f1xx.h"

/**
 * @brief Function to initialize system clock using HSI as the clock source.
 *
 * This function configures the system to use High-Speed Internal (HSI) as the clock source,
 * and sets up the PLL (Phase-Locked Loop) to enhance the system clock.
 * It will configure the following:
 * 1. Enable HSI (High-Speed Internal) oscillator.
 * 2. Configure the PLL to use HSI as its input (no division).
 * 3. Set the PLL multiplier to 9 for the system clock.
 * 4. Switch the system clock to PLL once it is stable.
 *
 * After calling this function, the system clock will be running at a higher speed,
 * driven by the PLL configured from HSI, improving the performance of the MCU.
 */
void Clock_Init(void) {
    // Bật HSI (High-Speed Internal) Oscillator
    RCC->CR |= RCC_CR_HSION;      // Bật HSI
    while (!(RCC->CR & RCC_CR_HSIRDY)) {}  // Chờ HSI ổn định

    // Cấu hình PLL để sử dụng HSI làm nguồn cho PLL (không chia đôi)
    RCC->CFGR |= RCC_CFGR_PLLSRC;  // Chọn HSI làm nguồn cho PLL
    RCC->CFGR |= RCC_CFGR_PLLMULL9; // Nhân PLL với 9 để có tốc độ xung cao hơn
    RCC->CR |= RCC_CR_PLLON;       // Bật PLL
    while (!(RCC->CR & RCC_CR_PLLRDY)) {}  // Chờ PLL ổn định

    // Chuyển hệ thống clock sang PLL
    RCC->CFGR |= RCC_CFGR_SW_PLL;  // Chọn PLL làm nguồn clock cho hệ thống

    // Đảm bảo hệ thống đang sử dụng PLL
    while ((RCC->CFGR & RCC_CFGR_SWS) != RCC_CFGR_SWS_PLL) {}
}
