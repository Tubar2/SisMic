#include <msp430.h> 
#include "libs/gpio.h"

/**
 * main.c
 */

void debounce(volatile int delay) {
    while (delay--);
}

unsigned char rand() {
    static unsigned char num = 5;
    num = (num * 17) % 7;
    return num;
}

void blinkLED(uint8_t port, uint8_t bit) {
    digitalWrite(port, bit, 1);
    debounce(0x9000);
    digitalWrite(port, bit, 0);
}

uint8_t check(uint8_t counter, uint8_t num) {
    if (counter != num) {
        blinkLED(1, 0);
        return 0;
    } else {
        blinkLED(4, 7);
        return 1;
    }
}

volatile uint8_t s1_flag = 0;
volatile uint8_t s2_flag = 0;

int main(void)
{
	WDTCTL = WDTPW | WDTHOLD;	// stop watchdog timer
	
	// Pin configuration
	pinConfig(1, 0, output);            // red LED_01 (P1.0) as exit
	pinConfig(4, 7, output);            // green LED_02 (P4.7) as exit
	pinConfig(2, 1, inPullUp);          // button S1 (P2.1) as input with pull-Up resistor
	pinConfig(1, 1, inPullUp);          // button S2 (P1.1) as input with pull-Up resistor

	// Confiura interrupção sendo que
	// haverá interrupção quando o
	// botão vai de 0 -> 1
	configInterruptible(2, 1, low_high);
	configInterruptible(1, 1, low_high);

	uint8_t counter = 0;
	uint8_t num = rand();

	// Habilita interrupções na CPU
	__enable_interrupt();

	while(1){
	    if (s1_flag) {
	        counter++;
	        if (counter > 6) {
	            counter = 0;
	        }
	        s1_flag = 0;
	        debounce(0x9999);
	    }
	    if (s2_flag) {
	        if (check(counter, num)){
	            counter = 0;
	            num = rand();
	        }
	        s2_flag = 0;
	        debounce(0x9999);
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
// Rotina de tratamento da interrupção pelo botão s2 (P1.1)
#pragma vector=PORT1_VECTOR
__interrupt void PORT1_ISR(void) {
    switch (_even_in_range(P1IV, 0x10)) {
        case 0x00: break;
        case 0x02: break;
        case 0x04:
            s2_flag = 1;
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
