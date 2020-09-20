#include <msp430.h> 
#include "libs/gpio.h"

/**
 * main.c
 */
int main(void)
{
	WDTCTL = WDTPW | WDTHOLD;	// stop watchdog timer
	
	// Pin configuration
	pinConfig(1, 0, output);            // Green LED (P4.7) as exit
	pinConfig(2, 1, inPullUp);          // BTN 2 as input with pull-Up resistor

	uint8_t prev = 0;

	while(1){
	    if (!digitalRead(2, 1)){        // ReadBTN and runs on btn pressed
	        prev = ~prev;
	        digitalWrite(1, 0, prev);   // Switch LED without rebound
	    }
	}

	return 0;
}
