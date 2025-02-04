#include "MKL25Z4.h"
#include "tsi.h"
#include <stdio.h>
#include "gpio.h"
#define TSI_THRESHOLD 105


void TSI_Init(void) {
    SIM->SCGC5 |= SIM_SCGC5_TSI_MASK;  
    TSI0->GENCS = TSI_GENCS_MODE(0) |   // Mod simplu
                  TSI_GENCS_REFCHRG(4) |
                  TSI_GENCS_DVOLT(1) |
                  TSI_GENCS_EXTCHRG(7) |
                  TSI_GENCS_PS(4) |
                  TSI_GENCS_NSCN(11) |
                  TSI_GENCS_TSIIEN_MASK |
                  TSI_GENCS_TSIEN_MASK;  // Activare TSI
}

uint16_t TSI_Read_X(void) {
    TSI0->DATA = TSI_DATA_TSICH(10);  // Selectare canal X
    TSI0->DATA |= TSI_DATA_SWTS_MASK;  // Start masurare
    while (!(TSI0->GENCS & TSI_GENCS_EOSF_MASK));  // Asteptare finalizare
    TSI0->GENCS |= TSI_GENCS_EOSF_MASK;  // Resetare flag
    return TSI0->DATA & TSI_DATA_TSICNT_MASK;  // Returnare valoare
}

uint16_t TSI_Read_Y(void) {
    TSI0->DATA = TSI_DATA_TSICH(11);  // Selectare canal Y
    TSI0->DATA |= TSI_DATA_SWTS_MASK;  // Start masurare
    while (!(TSI0->GENCS & TSI_GENCS_EOSF_MASK));  // Asteptare finalizare
    TSI0->GENCS |= TSI_GENCS_EOSF_MASK;  // Resetare flag
    return TSI0->DATA & TSI_DATA_TSICNT_MASK;  // Returnare valoare
}

uint8_t Process_TSI(void) {
    static uint16_t previous_x = 0, previous_y = 0;
    static int8_t last_dir_x = 0, last_dir_y = 0; 
    static uint8_t swipe_count = 0;
    static char active_axis = 0; 

    uint16_t current_x = TSI_Read_X();
    uint16_t current_y = TSI_Read_Y();

    int8_t current_dir_x = 0;
    int8_t current_dir_y = 0;

    if (current_x > previous_x + TSI_THRESHOLD) {
        current_dir_x = 1; // dreapta
    } else if (current_x < previous_x - TSI_THRESHOLD) {
        current_dir_x = -1; // stânga
    }

    if (current_y > previous_y + TSI_THRESHOLD) {
        current_dir_y = 1; // sus
    } else if (current_y < previous_y - TSI_THRESHOLD) {
        current_dir_y = -1; // jos
    }

    // Logica axei active:
    // Daca detectam o miscare pe X si nu exista miscare semnificativa pe Y, lucram pe axa X.
    // Daca detectam miscare pe Y si nu exista miscare semnificativa pe X, lucram pe axa Y.
    // Pentru simplitate, daca exista mi?care pe X, prioritizam X, altfel Y.
    char new_axis = 0;
    if (current_dir_x != 0 && current_dir_y == 0) {
        new_axis = 'X';
    } else if (current_dir_y != 0 && current_dir_x == 0) {
        new_axis = 'Y';
    } else if (current_dir_x == 0 && current_dir_y == 0) {
        // Nicio directie clara
        previous_x = current_x;
        previous_y = current_y;
        return 0; 
    }
		else {
			// in caz de miscare pe diagonala
			swipe_count = 0;
			new_axis = 0;  
	}

    // Daca axa activa s-a schimbat fata de cea anterioara, resetam contorul
    if (new_axis != active_axis) {
        swipe_count = 0;
        active_axis = new_axis;
        // Resetam si directiile anterioare
        last_dir_x = current_dir_x;
        last_dir_y = current_dir_y;
    } else {
    
        // In functie de axa activa, vedem daca directia s-a mentinut sau s-a schimbat
        if (active_axis == 'X') {
            if (current_dir_x != 0) {
                // Daca directia s-a schimbat fata de ultima directie
                if (current_dir_x != last_dir_x && last_dir_x != 0) {
                    // Avem o schimbare de directie pe aceeati axa
                    uint8_t result = swipe_count; 
                    // Resetam contorul si actualizam directia
                    swipe_count = 0;
                    last_dir_x = current_dir_x;
                    previous_x = current_x;
                    previous_y = current_y;
                    return result; // Returnam numarul de swipe-uri detectate înainte de schimbare
                } else {
                    // Daca directia e aceeasi, incrementam contorul
                    swipe_count++;
                    last_dir_x = current_dir_x;
                }
            } else {
                // Directia X a disparut (nu mai este miscare pe axa X)
							last_dir_x = 0;
            }
        } else if (active_axis == 'Y') {
            if (current_dir_y != 0) {
                if (current_dir_y != last_dir_y && last_dir_y != 0) {
                    uint8_t result = swipe_count;
                    swipe_count = 0;
                    last_dir_y = current_dir_y;
                    previous_x = current_x;
                    previous_y = current_y;
                    return result; 
                } else {
                    swipe_count++;
                    last_dir_y = current_dir_y;
                }
            } else {
                // Directia Y a disparut
                last_dir_y=0;
            }
        }
    }

    previous_x = current_x;
    previous_y = current_y;

    return 0; 
}
