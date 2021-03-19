/*
 * escravo.c
 *
 *  Created on: Nov 3, 2020
 *      Author: ricardosantos
 */


#include <msp430.h>
#include <stdint.h>
#include "libs/gpio.h"

// Escravo SPI
#define USCI_SPI_RXIFG     0x02
#define USCI_SPI_TXIFG     0x04

#define APAGADO 0
#define LIGADO 548

/* Variáveis:
 * - Status caso algum botão tenha sido pressionado ou não
 * - Potência do verde
 * - Potência do vermelho
 */
volatile uint8_t btnStatus, green, red;

void configEscravo()
{
    /* Configuração dos LEDs e botões */
    pinConfig(1, 0, output);            // LED Vermelho
    pinConfig(4, 7, output);            // LED Verde
    pinConfig(1, 1, inPullUp);          // SW2 Direito: com pull up
    pinConfig(2, 1, inPullUp);          // SW1 Esquerdo: com pull up

    /* Configuração da interface como escravo */
    /*          Escravo -> UCB-0              */
    // 1. Reseta a interface
    UCB0CTLW0 = UCSWRST;

    // 2. Configuração: Polaridade, MSB-first, escravo e sincrono
    UCB0CTLW0 |= 0x2100;
    P3SEL |= (BIT2 | BIT1 | BIT0); // P3.2(clk), P3.1(MISO), P3.0(MOSI)

    // 3. Liberando a interface
    UCB0CTLW0 &= ~UCSWRST;

    // 4. Habilitar interrupções em bytes recebidos
    UCB0IE = UCRXIE;

    /* Configuração do timer para geração da PWM */
    /* Frequencia de 60 Hz*/
    // 1. Configura o timer com Aclk, modo UP, e habilita interrupções
    TA0CTL = TASSEL__ACLK | MC__UP | TAIE;

    // 2. Configura a frequência do sinal
    TA0CCR0 = 546-1;          // freq => 32_768 / 60Hz = 546

    // 3. Configura o duty-cycle
    // CCR1 -> LED Vermelho
    TA0CCR1 = APAGADO;              // Começa apagado
    TA0CCTL1 = CCIE;                // Habilita interrupções

    // CCR2 -> LED Verde
    TA0CCR2 = APAGADO;              // Começa apagado
    TA0CCTL2 = CCIE;                // Habilita interrupções

}

/* Interrupções */
#pragma vector = TIMER0_A1_VECTOR
__interrupt void TA0_ISR(){
    switch(TA0IV){
    case 2:                     // CCR1
        P1OUT &= ~BIT0;         // Desliga o led vermelho
        break;
    case 4:                     // CCR2
        P4OUT &= ~BIT7;         // Desliga o led verde
        break;
    case 14:                    // Overflow
        if(green){              // Liga apenas se potência de verde > 0
            P4OUT |= BIT7;
        }
        if (red){
            P1OUT |= BIT0;      // Liga apenas se potência de vermelho > 0
        }
        break;
    }
}

#pragma vector = USCI_B0_VECTOR
__interrupt void SPI_ISR(){
    switch (UCB0IV){
        case 0x02:      // RX
            // Byte dummy
            if (UCB0RXBUF == 0xFE){
                return;
            }
            // Caso o comando recebido tenha sido de ler os botões
            if (UCB0RXBUF == 0xFF){
                btnStatus = 0;
                btnStatus = BIT0 & !digitalRead(1, 1);          // Ler SW2 Direito
                btnStatus <<= 4;
                btnStatus |= BIT0 & !digitalRead(2, 1);          // Ler SW1 esquerdo

                // Na próxima transferência, será lido o estado dos botões no mestre
                UCB0TXBUF = btnStatus;
            } else {
                green = UCB0RXBUF & 0xF0;   // Lê a potência solicitada em verde
                red = UCB0RXBUF & 0x0F;     // Lê a potência solicitada em vermelho
                switch(green){
                // Apagado
                case 0x00:
                    TA0CCR2 = APAGADO;
                    break;
                // 25%
                case 0x10:
                    TA0CCR2 = 137 - 1;
                    break;
                // 50%
                case 0x20:
                    TA0CCR2 = 273 - 1;
                    break;
                // 75%
                case 0x30:
                    TA0CCR2 = 410 - 1;
                    break;
                // Ligado
                case 0x40:
                    TA0CCR2 = LIGADO;   // Tx_CCRy > Tx_CCR0 -> Nunca apaga
                    break;
                }
                switch(red){
                // Apagado
                case 0x00:
                    TA0CCR1 = APAGADO;
                    break;
                    // 25%
                case 0x1:
                    TA0CCR1 = 137 - 1;
                    break;
                    // 50%
                case 0x2:
                    TA0CCR1 = 273 - 1;
                    break;
                    // 75%
                case 0x3:
                    TA0CCR1 = 410 - 1;
                    break;
                    // Ligado
                case 0x4:
                    TA0CCR1 = LIGADO;   // Tx_CCRy > Tx_CCR0 -> Nunca apaga
                    break;
                }
            }

        break;
        default:
        break;
    }
}

