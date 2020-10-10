#include <msp430.h> 
#include "libs/gpio.h"
#include "libs/timer.h"

/**
 * main.c
 */


int main(void)
{
	WDTCTL = WDTPW | WDTHOLD;	// stop watchdog timer
	
	pinConfig(1, 0, output);    // LED01 vermelho como saída

	const uint16_t n = 20971;   // Frequência de 50Hz de piscagem n = Fclk/50Hz
	const uint16_t n2 = 10486;  // Duty cycle de 50% n2 = 0.5 * n

	// Endereço CTL, CCR0 e CCR1 de TA_0
	uint16_t * timerCTL = getTimerRegstr(TA_0, CTL, 0);
	uint16_t * timerCCR0 = getTimerRegstr(TA_0, CCR, 0);
	uint16_t * timerCCR1 = getTimerRegstr(TA_0, CCR, 1);

	// Configurando timerA0 com SMCLK e contagem UP
	configTimer(timerCTL, SMCLK, UP, 0);

	// Interrupções em n e n2
	setInterruptionAt(timerCCR0, n);
	setInterruptionAt(timerCCR1, n2);

	// Pegando os registradores CCTL 0 e 1 do timerA
	uint16_t * timerCCTL0 = getTimerRegstr(TA_0, CCTL, 0);
	uint16_t * timerCCTL1 = getTimerRegstr(TA_0, CCTL, 1);
	while(1){

	    // Apaga o LED
	    while(!(*timerCCTL1 & CCIFG));
	    *timerCCTL1 &= ~CCIFG;
	    P1OUT &= ~BIT0;

	    // Acende o LED
	    while(!(*timerCCTL0 & CCIFG));
	    *timerCCTL0 &= ~CCIFG;
	    P1OUT |= BIT0;
	}

}


