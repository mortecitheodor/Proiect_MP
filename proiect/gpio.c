#include "gpio.h"
#include "MKL25Z4.h"
#include <stdint.h>

#define SWITCH_PIN (12) // PORT A
#define RED_LED_PIN (18) // PORT B
#define GREEN_LED_PIN (19) // PORT B
#define BLUE_LED_PIN (1)  // PORT D
#include "ClockSettings.h"


void RGBLed_Init(void) {
    SIM->SCGC5 |= SIM_SCGC5_PORTB_MASK | SIM_SCGC5_PORTD_MASK;

    PORTB->PCR[RED_LED_PIN] &= ~PORT_PCR_MUX_MASK;
    PORTB->PCR[RED_LED_PIN] |= PORT_PCR_MUX(1);
    GPIOB_PDDR |= (1 << RED_LED_PIN);  
    GPIOB_PSOR |= (1 << RED_LED_PIN);  

    PORTB->PCR[GREEN_LED_PIN] &= ~PORT_PCR_MUX_MASK;
    PORTB->PCR[GREEN_LED_PIN] |= PORT_PCR_MUX(1);
    GPIOB_PDDR |= (1 << GREEN_LED_PIN);  
    GPIOB_PSOR |= (1 << GREEN_LED_PIN);  

    PORTD->PCR[BLUE_LED_PIN] &= ~PORT_PCR_MUX_MASK;
    PORTD->PCR[BLUE_LED_PIN] |= PORT_PCR_MUX(1);
    GPIOD_PDDR |= (1 << BLUE_LED_PIN);  
    GPIOD_PSOR |= (1 << BLUE_LED_PIN);  
	
		
}

void Change_LED_state(uint32_t state){
	switch (state) {
    case 0:  //alb
        GPIOB_PCOR |= (1 << RED_LED_PIN);
        GPIOB_PCOR |= (1 << GREEN_LED_PIN);
        GPIOD_PCOR |= (1 << BLUE_LED_PIN);
        break;

    case 1:  //albastru
        GPIOB_PSOR |= (1 << RED_LED_PIN);
				GPIOB_PSOR |= (1 << GREEN_LED_PIN);
				GPIOD_PCOR |= (1 << BLUE_LED_PIN);
        break;

    case 2: //albastru deschis
        GPIOB_PSOR |= (1 << RED_LED_PIN);
				GPIOB_PCOR |= (1 << GREEN_LED_PIN);
				GPIOD_PCOR |= (1 << BLUE_LED_PIN);
        break;

    case 3:
        GPIOB_PSOR |= (1 << RED_LED_PIN);
        GPIOB_PSOR |= (1 << GREEN_LED_PIN);
        GPIOD_PSOR |= (1 << BLUE_LED_PIN);
        state = 0;
        break;

   
}


	
}
void LED_Sequence(uint32_t swipe_counter){
	
	num_of_seconds_passed=0;
	uint32_t state=0;

		
	while(num_of_seconds_passed<swipe_counter){
		
		  if (base_tick_373ms >= 373U) {
        base_tick_373ms = 0U;
				Change_LED_state(state);
				state++;
    }
		state=state%4;
	
	
	}
	 GPIOB_PSOR = (1 << RED_LED_PIN) | (1 << GREEN_LED_PIN);
   GPIOD_PSOR = (1 << BLUE_LED_PIN);
	
	
}
