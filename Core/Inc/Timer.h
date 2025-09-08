#ifndef TIMER_H
#define TIMER_H

#include <stdint.h>
#include "Timer_Register.h"

/* API cho SysTick Timer */
void Timer_Init(uint32_t ticks);
void Timer_Start(void);
void Timer_Stop(void);
void Timer_DelayUs(uint32_t us);
void Timer_DelayMs(uint32_t ms);

/* Callback gọi trong SysTick_Handler (do CubeMX tạo) */
void Timer_Tick(void);

#endif /* TIMER_H */
