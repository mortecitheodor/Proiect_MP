#include "MKL25Z4.h"
#include "gpio.h"
#include "uart.h"
#include "tsi.h"
#include "Pit.h"
#include <stdio.h>  

int main(void) {
    RGBLed_Init();        
    UART0_Initialize(38400);
    TSI_Init();
    PIT_Init();

    while (1) {
        uint8_t swipe_count = Process_TSI();
        if (swipe_count > 0) {
            char buffer[50];
            sprintf(buffer,"Num. of Swipes: %d\r\n", swipe_count);
            UART0_Print(buffer);  

            LED_Sequence(swipe_count);
        }
    }

    return 0;
}
