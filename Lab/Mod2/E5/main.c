#include <msp430.h> 
#include "libs/gpio.h"

/**
 * main.c
 */

void debounce() {
    volatile int delay = 0x9999;
    while (delay--);
}

volatile uint8_t s1_flag = 0;

int main(void)
{
	WDTCTL = WDTPW | WDTHOLD;	// stop watchdog timer
	
	// Pin configuration
	pinConfig(1, 0, output);            // red LED_01 (P1.0) as exit
	pinConfig(2, 1, inPullUp);          // button S1 (P2.1) as input with pull-Up resistor

	// Confiura interrupção sendo que
	// haverá interrupção quando o
	// botão vai de 0 -> 1
	configInterruptible(2, 1, low_high);

	uint8_t prev = 0;

	// Habilita interrupções na CPU
	__enable_interrupt();

	while(1){
	    if (s1_flag) {
	        prev = ~prev;
	        digitalWrite(1, 0, prev);
	        s1_flag = 0;
	        debounce();
	    }
	}

}

// Rotina de tratamento da interrupção pelo botão s1 (P2.1)
#pragma vector=PORT2_VECTOR
__interrupt void PORT2_ISR(void) {
    switch (_even_in_range(P2IV, 0x10)) {
        case 0x00: break;
        case 0x02: break;
        case 0x04:
            s1_flag = 1;
            break;
        case 0x06: break;
        case 0x08: break;
        case 0x0A: break;
        case 0x0C: break;
        case 0x0E: break;
        case 0x10: break;
        default: break;
    }
}
