#include "MKL25Z4.h"
#include "gpio.h"
#include "uart.h"
#include "tsi.h"
#include "ClockSettings.h"
#include <stdio.h>  
#include "uart.h"

int main(void) {
    RGBLed_Init();        
    TSI_Init();
		SystemClockTick_Configure();
		UART0_Initialize(9600);

    while (1) {
        uint8_t swipe_count = Process_TSI();
        if (swipe_count > 0) {
            char buffer[50];
            sprintf(buffer,"num. of swipes: %d\r\n", swipe_count);
            UART0_Print(buffer); 
						LED_Sequence(swipe_count);
        }
    }

    return 0;
}
