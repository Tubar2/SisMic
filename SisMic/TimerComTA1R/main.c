#include <msp430.h> 
#include "libs/gpio.h"

/**
 * main.c
 */


int main(void)
{
	WDTCTL = WDTPW | WDTHOLD;	// stop watchdog timer
	
	// Configurar os pinos
	pinConfig(1, 0, output);    // LED vermelho como saída


	// Configurar o timer
	TA1CTL = TASSEL__ACLK   |
	        MC__CONTINUOUS  |
	        TACLR;


	while(1){
	    // Lendo o timer
	    if(TA1R > 0x8000){
	        digitalWrite(1, 0, 1);
	    } else {
	        digitalWrite(1, 0, 0);
	    }
	}

}
