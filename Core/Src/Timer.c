#include "Timer.h"

static volatile uint32_t usTicks = 0;

/* Callback: được gọi trong SysTick_Handler */
void Timer_Tick(void)
{
    if (usTicks != 0) {
        usTicks--;
    }
}

void Timer_Init(uint32_t ticks)
{
    /* Set reload value */
    SYSTICK->RVR = ticks - 1;

    /* Reset current value */
    SYSTICK->CVR = 0;

    /* Chọn clock source: processor clock (AHB) */
    SYSTICK->CSR |= SYST_CSR_CLKSOURCE;

    /* Enable SysTick interrupt */
    SYSTICK->CSR |= SYST_CSR_TICKINT;
}

void Timer_Start(void)
{
    SYSTICK->CSR |= SYST_CSR_ENABLE;
}

void Timer_Stop(void)
{
    SYSTICK->CSR &= ~SYST_CSR_ENABLE;
}

void Timer_DelayUs(uint32_t us)
{
    usTicks = us;
    Timer_Start();
    while (usTicks);
    Timer_Stop();
}

void Timer_DelayMs(uint32_t ms)
{
    while (ms--) {
        Timer_DelayUs(1000);
    }
}
