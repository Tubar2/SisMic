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
	pinConfig(1, 3, output);    //Pino 1.3 como saída

	P1SEL |= BIT3;              // Conecta o timer A0 e contador 2 (CCR2) ao pino 1.3

	const uint16_t n = 20971;   // Frequência de 50Hz de piscagem n = Fclk/50Hz
	const uint16_t n2 = 10486;  // Duty cycle de 50% n2 = 0.5 * n

	// Registradores CTL, CCR0 e CCR2
	uint16_t * timerCTL = getTimerRegstr(TA_0, CTL, 0);
	uint16_t * timerCCR0 = getTimerRegstr(TA_0, CCR, 0);
	uint16_t * timerCCR2 = getTimerRegstr(TA_0, CCR, 2);

	// Configurando timerA0 com SMCLK e contagem UP
	configTimer(timerCTL, SMCLK, UP, 0);

    // Interrupções em n e n2
	setInterruptionAt(timerCCR0, n);
	setInterruptionAt(timerCCR2, n2);

	// Configurando
	TA0CCTL2 = OUTMOD_7;
	uint16_t * timerCCTL0 = getTimerRegstr(TA_0, CCTL, 0);

	while(1){
	    // Acende o LED
	    while(!(*timerCCTL0 & CCIFG));
	    *timerCCTL0 &= ~CCIFG;
	    P1OUT |= BIT0;
	}

}


