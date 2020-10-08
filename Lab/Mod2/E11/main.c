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
	pinConfig(4, 7, output);    // LED verde como saída
	pinConfig(1, 0, output);    // LED vermelho como saída

	// Configurar interrupções
	// 1. Obter endereço de TA0CTL e dos CCR's 1 e 2
	uint16_t * timerCTL = getTimerRegstr(TA_0, CTL, 0);
	uint16_t * timerCCR1 = getTimerRegstr(TA_0, CCR, 1);
	uint16_t * timerCCR2 = getTimerRegstr(TA_0, CCR, 2);

	// 2. Configura TA0CTL com SMCLK e modo de contagem UP
	configTimer(timerCTL, ACLK, Continuous, 0);

	// 3. Adiciona interrupções em
	uint16_t oneHz = 0x4000;        // 1 Hz
	uint16_t twoHz = 0x2000;        // 2 Hz

	setInterruptionAt(timerCCR1, oneHz);
	setInterruptionAt(timerCCR2, twoHz);

	uint16_t * timerCCTL1 = getTimerRegstr(TA_0, CCTL, 1);
	uint16_t * timerCCTL2 = getTimerRegstr(TA_0, CCTL, 2);
	// 4. Loop que checa pelas flags de interrupção dos comparadores
	while(1){

	    if(*timerCCTL2 & CCIFG){
	        P4OUT ^= BIT7;
	        *timerCCTL2 &= ~CCIFG;
	    }
	    if (*timerCCTL1 & CCIFG){
	        P1OUT ^= BIT0;
	        P4OUT ^= BIT7;
	        *timerCCTL1 &= ~CCIFG;
	        *timerCTL |= TACLR;
	    }

	}

}
