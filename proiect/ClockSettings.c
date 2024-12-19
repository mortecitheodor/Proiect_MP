#include "ClockSettings.h"

volatile uint16_t base_tick_373ms = 0U;
uint16_t base_tick_1s   = 0U;
volatile uint32_t num_of_seconds_passed;

void SystemClock_Configure(void) {
    
    MCG->C1 |= MCG_C1_CLKS(0);      
    MCG->C1 |= MCG_C1_IREFS_MASK;   
    MCG->C4 |= MCG_C4_DRST_DRS(1);
    MCG->C4 |= MCG_C4_DMX32(1);
}

void SystemClockTick_Configure(void) {
    SysTick->LOAD = (uint32_t)(48000000UL / 1000UL - 1UL);
    
    NVIC_SetPriority(SysTick_IRQn, (1UL << __NVIC_PRIO_BITS) - 1UL);
    
   
    SysTick->VAL = 0UL;
    
    SysTick->CTRL |= (SysTick_CTRL_CLKSOURCE_Msk | 
                      SysTick_CTRL_TICKINT_Msk   |
                      SysTick_CTRL_ENABLE_Msk);
}

void SysTick_Handler(void) {
    ++base_tick_373ms;
    ++base_tick_1s;

    if (base_tick_1s >= 1000U) {
        base_tick_1s = 0U;
			num_of_seconds_passed++;
 
    }
}
