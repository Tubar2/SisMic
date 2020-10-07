#include <msp430.h> 
#include "libs/gpio.h"
#include "libs/timer.h"

/**
 * main.c
 */


int main(void)
{
	WDTCTL = WDTPW | WDTHOLD;	// stop watchdog timer
	

	// Configurar os pinos
	pinConfig(4, 7, output);    // LED vermelho como saída

	while(1){

	    wait(500, ms, TA_2);

	    P4OUT ^= BIT7;

	}

}
