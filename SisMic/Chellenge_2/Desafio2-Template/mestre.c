#include <msp430.h> 
#include <stdint.h>

// Comunicacao Mestre/Escravo SPI
void configEscravo();

void main()
{
    WDTCTL = WDTPW | WDTHOLD;

    /* Configuracao do mestre */


    /* Configuracao do escravo */
    configEscravo();

    /* Loop infinito */
    while(1)
    {
    	/*
    	 * Faz a leitura dos botões do escravo e  
    	 * ajusta a luminosidade dos LEDs do escravo.
    	 */

    }

}










