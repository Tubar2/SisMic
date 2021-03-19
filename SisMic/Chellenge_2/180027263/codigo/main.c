#include <msp430.h> 
#include <stdint.h>
#include "libs/spi.h"
#include "libs/timer.h"

// Comunicacao Mestre/Escravo SPI
void configEscravo();

/* Variáveis:
 * - Contadores para número de vezes que SW1 e SW2 foram apertados
 * - Comando a ser enviado para o escravo
 * - Status caso algum botão foi pressioando
 */
volatile uint8_t SW1_Counter, SW2_Counter, cmd, btStatus;

void main()
{
    WDTCTL = WDTPW | WDTHOLD;

    /* Configuracao do mestre */
    /*    Mestre -> UCB-1     */
    // 1. Reseta a interface
    UCB1CTLW0 = UCSWRST;

    // 2. Configuração: Polaridade, MSB-first, mestre e sincrono
    UCB1CTLW0 |= 0x2900 | UCSSEL__SMCLK;
    UCB1BRW = 10; // Fclk = 1Mhz/10 = 100kHz
    P4SEL |= (BIT3 | BIT2 | BIT1); // P4.3(clk), P4.2(MISO), P4.1(MOSI)

    // 3. Liberando a interface
    UCB1CTLW0 &= ~UCSWRST;

    /* Configuracao do escravo */
    configEscravo();

    // Habilitar interrupções globais
    __enable_interrupt();

    /* Loop infinito */
    while(1)
    {
        /*
         * Faz a leitura dos botıes do escravo e
         * ajusta a luminosidade dos LEDs do escravo.
         */
        // Pede a leitura dos pinos com o comando 0xFF (lê apenas 1 por vez)
         spiTransfer(0xFF);

         // Recebe a leitura dos pinos enviando um byte dummy
         btStatus = spiTransfer(0xFE);

         // Caso algum botão tenha sido pressionado:
         if(btStatus){
             // Remove rebote esperando 150ms no contador TA_1
             wait(150, ms, TA_1);
             switch(btStatus){
             // SW1 foi pressionado
             case 0x01:
                 if (SW1_Counter == 4) SW1_Counter = 0;// Reseta o contador do SW1
                 else SW1_Counter++;                   // Incrementa contador do SW1
                 break;
             // SW2 foi pressionado
             case 0x10:
                 if (SW2_Counter == 4) SW2_Counter = 0;// Reseta o contador do SW2
                 else SW2_Counter++;                   // Incrementa contador do SW2
                 break;
             default:
                 break;
             }
             // Formando o comando e transferindo para o escravo
             cmd = SW2_Counter;                             // cmd = xxxx-0ggg
             cmd <<= 4;                                     // cmd = 0ggg-0000
             cmd |= SW1_Counter;                            // cmd = 0ggg-0rrr
             spiTransfer(cmd);
         }

    }

}








