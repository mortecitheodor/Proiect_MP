#include "MKL25Z4.h"

extern volatile uint32_t num_of_seconds_passed;
extern volatile uint16_t base_tick_373ms;


void SystemClockTick_Configure(void);
void SysTick_Handler(void);

