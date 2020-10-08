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
	const uint16_t oneHz = 0x4000;        // 1 Hz -> CCR1
	const uint16_t twoHz = 0x2000;        // 2 Hz -> CCR2

	setInterruptionAt(timerCCR1, oneHz);
	setInterruptionAt(timerCCR2, twoHz);

	// 4. Habilitando interrupçoes nos registradores CCTL 1 e 2
	// setando os bits IE (bit4)
    uint16_t * timerCCTL1 = getTimerRegstr(TA_0, CCTL, 1);
    uint16_t * timerCCTL2 = getTimerRegstr(TA_0, CCTL, 2);
	setupTimerRegister(timerCCTL1, 1, 4);
	setupTimerRegister(timerCCTL2, 1, 4);

	// 5. Habilitando interrupções na CPU
	__enable_interrupt();

	// 6. Loop infinito
	while(1){
	    // Interrupçoões são geradas e as interrupções são chamadas sem a
	    // necessidade de chamá-la
	}
}

// Interrupços nos ccrs 0 a 5
#pragma vector = TIMER0_A1_VECTOR
__interrupt void TA0_CCRN_ISR(){
    switch(TA0IV){
    case 0x02:                      // CCR1
        P1OUT ^= BIT0;
        P4OUT ^= BIT7;
        TA0CTL |= TACLR;
        break;
    case 0x04:                      // CCR2
        P4OUT ^= BIT7;
        break;
    default:
        break;
    }
}


