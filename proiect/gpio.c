#include "gpio.h"
#include "Pit.h"
#include "MKL25Z4.h"
#include <stdint.h>

#define SWITCH_PIN (12) // PORT A
#define RED_LED_PIN (18) // PORT B
#define GREEN_LED_PIN (19) // PORT B
#define BLUE_LED_PIN (1)  // PORT D

void Switch_Init(void) {
    SIM->SCGC5 |= SIM_SCGC5_PORTA_MASK;
    PORTA->PCR[SWITCH_PIN] &= ~PORT_PCR_MUX_MASK;
    PORTA->PCR[SWITCH_PIN] |= PORT_PCR_MUX(1);
    PORTA->PCR[SWITCH_PIN] &= ~PORT_PCR_PS_MASK;
    PORTA->PCR[SWITCH_PIN] |= PORT_PCR_IRQC(0x09) | PORT_PCR_PE_MASK;
    NVIC_ClearPendingIRQ(PORTA_IRQn);
    NVIC_SetPriority(PORTA_IRQn, 128);
    NVIC_EnableIRQ(PORTA_IRQn);
}

void PORTA_IRQHandler() {
    static uint8_t state = 0;

    if(state == 0) {
        GPIOB_PCOR |= (1<<RED_LED_PIN);
        state = 1;
    } else if (state == 1) {
        GPIOB_PSOR |= (1<<RED_LED_PIN);
        GPIOB_PCOR |= (1<<GREEN_LED_PIN);
        state = 2;
    } else if (state == 2) {
        GPIOB_PSOR |= (1<<GREEN_LED_PIN);
        GPIOD_PCOR |= (1<<BLUE_LED_PIN);
        state = 3;
    } else if (state == 3) {
        GPIOD_PSOR |= (1<<BLUE_LED_PIN);
        GPIOB_PSOR |= (1<<RED_LED_PIN);
        GPIOB_PSOR |= (1<<GREEN_LED_PIN);
        state = 0;
    }
    PORTA_ISFR = (1<<SWITCH_PIN);
}

void RGBLed_Init(void){
    SIM->SCGC5 |= SIM_SCGC5_PORTB_MASK | SIM_SCGC5_PORTD_MASK;

    PORTB->PCR[RED_LED_PIN] &= ~PORT_PCR_MUX_MASK;
    PORTB->PCR[RED_LED_PIN] |= PORT_PCR_MUX(1);
    GPIOB_PDDR |= (1<<RED_LED_PIN);
    GPIOB_PSOR |= (1<<RED_LED_PIN);

    PORTB->PCR[GREEN_LED_PIN] &= ~PORT_PCR_MUX_MASK;
    PORTB->PCR[GREEN_LED_PIN] |= PORT_PCR_MUX(1);
    GPIOB_PDDR |= (1<<GREEN_LED_PIN);
    GPIOB_PSOR |= (1<<GREEN_LED_PIN);

    PORTD->PCR[BLUE_LED_PIN] &= ~PORT_PCR_MUX_MASK;
    PORTD->PCR[BLUE_LED_PIN] |= PORT_PCR_MUX(1);
    GPIOD_PDDR |= (1<<BLUE_LED_PIN);
    GPIOD_PSOR |= (1<<BLUE_LED_PIN);
}

void LED_Sequence(uint8_t duration) {
    for (uint8_t i = 0; i < duration; i++) {
        switch (i % 4) {
            case 0: // Aprindere Alb
                GPIOB_PCOR |= (1 << RED_LED_PIN);
                GPIOB_PCOR |= (1 << GREEN_LED_PIN);
                GPIOD_PCOR |= (1 << BLUE_LED_PIN);
                break;
            case 1: // Aprindere Albastru Normal
                GPIOB_PSOR |= (1 << RED_LED_PIN);
                GPIOB_PSOR |= (1 << GREEN_LED_PIN);
                GPIOD_PCOR |= (1 << BLUE_LED_PIN);
                break;
            case 2: // Aprindere Albastru Deschis
                GPIOB_PCOR |= (1 << GREEN_LED_PIN);
                GPIOD_PCOR |= (1 << BLUE_LED_PIN);
                break;
            case 3: // Stingere LED-uri
                GPIOB_PSOR |= (1 << RED_LED_PIN);
                GPIOB_PSOR |= (1 << GREEN_LED_PIN);
                GPIOD_PSOR |= (1 << BLUE_LED_PIN);
                break;
        }
				PIT_Wait_One_Second();
    }

    // Stingere LED
    GPIOB_PSOR |= (1 << RED_LED_PIN);
    GPIOB_PSOR |= (1 << GREEN_LED_PIN);
    GPIOD_PSOR |= (1 << BLUE_LED_PIN);
} 
