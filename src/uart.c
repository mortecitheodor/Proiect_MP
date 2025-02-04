#include "MKL25Z4.h"
#include "uart.h"

void UART0_Transmit(uint8_t data) {
    while(!(UART0->S1 & UART0_S1_TDRE_MASK));
    UART0->D = data;
}

uint8_t UART0_Receive(void) {
    while(!(UART0->S1 & UART0_S1_RDRF_MASK));
    return UART0->D;
}

void UART0_Initialize(uint32_t baud_rate) {
    uint16_t osr = 16;
    uint16_t sbr;

    SIM->SCGC4 |= SIM_SCGC4_UART0_MASK;   
    SIM->SCGC5 |= SIM_SCGC5_PORTA_MASK;  

    UART0->C2 &= ~(UART0_C2_RE_MASK | UART0_C2_TE_MASK);  
    SIM->SOPT2 |= SIM_SOPT2_UART0SRC(1);  

    PORTA->PCR[1] = PORT_PCR_ISF_MASK | PORT_PCR_MUX(2);  
    PORTA->PCR[2] = PORT_PCR_ISF_MASK | PORT_PCR_MUX(2);  

    sbr = (uint16_t)((DEFAULT_SYSTEM_CLOCK)/(baud_rate * osr));
    UART0->BDH &= ~UART0_BDH_SBR_MASK;
    UART0->BDH |= UART0_BDH_SBR(sbr >> 8);
    UART0->BDL = UART0_BDL_SBR(sbr);
    UART0->C4 |= UART0_C4_OSR(osr - 1);

    UART0->C1 = UART0_C1_M(0) | UART0_C1_PE(0);  
    UART0->S2 = UART0_S2_MSBF(0);
    UART0->C2 |= (UART0_C2_RE_MASK | UART0_C2_TE_MASK);  
}

void UART0_Print(const char *str) {
    while (*str) {
        UART0_Transmit(*str++);
    }
}
