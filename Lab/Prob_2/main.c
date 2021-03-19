// /Applications/ti/ccs1010/ccs/ccs_base/msp430/include
// #include "/Applications/ti/ccs1010/ccs/ccs_base/msp430/include/msp430.h"
#include <msp430.h> 
#include "libs/gpio.h"
#include "libs/timer.h"

/**
 * main.c
 */

void triggerSensor();
void blinkLed();

volatile uint8_t continuo;
volatile uint32_t delta;
volatile uint32_t prev;
volatile uint32_t dist;

int main(void)
{
	WDTCTL = WDTPW | WDTHOLD;	// stop watchdog timer
	
	pinConfig(1, 0, output);    // LED vermelho
	pinConfig(4, 7, output);    // LED verde

	// SW1 regula como serão feitas a medição da distância: contínuo ou disparos
	// O programa inicia com medidas feitas a partir de disparos de sw2
	// Aperto do butão será tratado como interrupção agrupada
	// Configura sw2 (P2.1) com uma interrupção quando pressionado
	// pinConfig(2, 1, inPullUp);
	// configInterruptibleAddr(2, 1, high_low);
	Pin sw1 = {2, 1};
	pinConfigPin(&sw1, inPullUp);
	configInterruptiblePin(&sw1, high_low);

	// SW2 ativa o sensor com disparos, ou seja, quando apertado, faz UMA medida
	Pin sw2 = {1,1};
	pinConfigPin(&sw2, inPullUp);	// Configurando P1.1 como saída com Pull Up

	// Pino 1.2 é o responsável por enviar um sinal à porta trigger do sensor
	pinConfig(1, 2, output);

	// Configura TA_0 para eliminar rebote da interrupção do botão
	const Timer t0CTL = {TA_0, CTL};
	const Timer t0CCTL = {TA_0, CCTL};
	const Timer t0CCR = {TA_0, CCR};
	ctlQuickConfig(t0CTL, Stop, ACLK);       // Configura TA0 com ACLK e parado
	setInterruptionAt(t0CCR, 0, 0xF000);     // Ativar interrupção em TA0CCR0: 0xF000
	cctlCompInteruptConfig(t0CCTL, 0, 1);    // Configura TA0CCTL0 para gerar interrupções

	/*
	 * Configurando a captura do sinal echo:
	 * O pino P2.0 irá fazer a captura de ambos of flancos do sinal
	 */
	// Configurar P2.0 para receber entradas do timer A1.1 (TA1.CCIA1)
	P2DIR &= ~BIT0;
	P2SEL |= BIT0;

	// Configurar o timer A1 para começar parado com ACLK (=32768Hz)
	const Timer t1CTL = {TA_1, CTL};
	ctlQuickConfig(t1CTL, Stop, ACLK);

	// Habilitar o modo de captura em TA1CCTL1 com interrupções
	// TA1CCTL1 = CM_3 | CAP | CCIE | SCS;  // Captura em ambos os flancos
	Timer t1CCTL = {TA_1, CCTL};
	uint16_t * t1CCTL1 = getTimerRegstrOff(t1CCTL, 1);
	cctlCapStdConfigAddr(t1CCTL1, bothEdges, cciA, 1, capture, 1);
	// Habilitar comparações para incrementar o overflow do timer

	_enable_interrupts();

	continuo = 0;
	delta = 0;
	while(1){
	    if(continuo){
			// Inicia o clock TA_1 que mede o tempo do sinal em modo continuo
	        TA1CTL |= MC__CONTINUOUS;

			while (continuo){
				// Começa o timer de 0
				TA1CTL |= TACLR;

				delta = 0;
	        	prev = 0;

	        	// A cada 0,5 segundos faz uma nova medida
	        	triggerSensor();

	        	// Remove o rebote e espera até que o sinal do sensor tenha cumprido o pecurso
	        	wait(25, ms, TA_2);

	        	// Calcula a distância do sensor ao objeto medido
	        	dist = (17000*delta)/32768;

	        	// Pisca os leds de acordo com a distância
	        	blinkLed();

	        	// Aguarda 0,5 sec para uma nova medição
	        	wait(500, ms, TA_2);

			}

	    } else if (!digitalRead(1, 1)) {
	        delta = 0;
	        // Inicia o clock TA_1 que mede o tempo do sinal
	        TA1CTL |= MC__CONTINUOUS | TACLR;

	        // Pressionar de SW1 mede uma distância
	        triggerSensor();
	        // Remove o rebote e espera até que o sinal do sensor tenha cumprido o pecurso
	        wait(25, ms, TA_2);

	        // Calcula a distância do sensor ao objeto medido
	        dist = (17000*delta)/32768;

	        // Pisca os leds de acordo com a distância
	        blinkLed();

	        // Desliga o clock para poupar a bateria
	        TA1CTL |= MC__STOP;
	    }
	}

}

void triggerSensor(){
    P1OUT |= BIT2;      // Envia um sinal para ativar trigger
    wait(20, us, TA_2); // Espera o tempo mínimo de 12 us do sinal trigger
    P1OUT &= ~BIT2;     // Finaliza o sinal
}

void blinkLed(){
    if (dist > 20 && dist < 40){
        // Liga o led verde por 0,5 sec
        P4OUT |= BIT7;
        wait(500, ms, TA_2);
        P4OUT &= ~BIT7;
    } else if(dist > 40 && dist < 60){
        // Liga o led vermelho por 0,5 sec
        P1OUT |= BIT0;
        wait(500, ms, TA_2);
        P1OUT &= ~BIT0;
    } else if(dist > 60){
        // Liga ambos os leds por 0,5 sec
        P1OUT |= BIT0;
        P4OUT |= BIT7;
        wait(500, ms, TA_2);
        P1OUT &= ~BIT0;
        P4OUT &= ~BIT7;
    }
}

// Trata interrupções no timer TA_0 em CCR0
#pragma vector = TIMER0_A0_VECTOR
__interrupt void TA0_CCR0_ISR(){
    P2IE |= BIT1;       // Reabilita interrupções no pino

    TA0CTL &= ~MC__UP;  // Para o timer

    P2IFG &= ~BIT1;     // Abaixa a flag de interrupção pendente no SW2
}

// Trata interrupções no timer TA_1 em CCRN
#pragma vector = TIMER1_A1_VECTOR
__interrupt void TA1_CCRN_ISR(){
    switch(TA1IV){
    case 2:             // Trata interrupções em CCR1
        prev = TA1CCR1;
        delta = prev - delta;
        break;
    default:
        break;
    }
}

// Trata interrupções no SW2
#pragma vector=PORT2_VECTOR
__interrupt void P2_ISR(){
    P2IE &= ~BIT1;              // Interrompe futuras interrupções pelo pino
                                // pelo tempo do timer
    TA0CTL |= MC__UP | TACLR;   // Inicia o timer
    continuo = !continuo;       // Alterna a medição entre continuo ou disparos

    P2IES ^= BIT1; // Talvez erro aqui

}

//#pragma vector = TIMER0_A0_VECTOR
//__interrupt void bla(){
//    tempo = TA0CCR0;
//    P4OUT |= BIT7;
//    flag_stop = 1;
//}


