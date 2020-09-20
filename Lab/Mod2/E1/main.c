#include <msp430.h> 
#include "libs/gpio.h"

/**
 * main.c
 */
int main(void)
{
	WDTCTL = WDTPW | WDTHOLD;	// stop watchdog timer
	
	// Pin configuration
	pinConfig(4, 7, output);        // Green LED (P4.7) as exit
	pinConfig(1,1, inPullUp);       // BTN 2 as input with pull-Up resistor

	while(1){
	    if (digitalRead(1, 1)){     // ReadBTN and returns 1 on not pressed
	        digitalWrite(4, 7, 0);  // Turns LED off if BTN not pressed
	    } else {
	        digitalWrite(4, 7, 1);  // Turn LED on if BTN pressed
	    }
	}

	return 0;
}
