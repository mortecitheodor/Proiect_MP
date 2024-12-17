#include "MKL25Z4.h"

void Switch_Init(void);
void RGBLed_Init(void);
void PORTA_IRQHandler(void);
void LED_Sequence(uint8_t duration);
void GPIO_PIT_IRQHandler(void);
