#include <msp430.h> 
#include <stdint.h>
#include <stdio.h>

/**
 * main.c
 */

/* Funções Auxiliares */
void uartInit();
void initADC();
void initDMA();
void printUartDma(char * str, uint8_t nBytes);
void uartPrint(char * str);
void uartSend(uint8_t byte);
uint8_t uartGet(char * data);
void printStats(uint8_t chan);

/* Variáveis Auxiliares */
uint16_t adcResult[4];              // Resultados da conversão AD
uint8_t rxBuff[256], rxWr, rxRd;    // Buffer de leitura do UART e ponteiros de escrita e leitura

int main(void) {
	WDTCTL = WDTPW | WDTHOLD;	// stop watchdog timer
	
	uartInit();             // Inicia a comunicação UART com BdRate = 38400
	initADC();              // Inicia o conversor AD com trigger de 16Hz
	initDMA();              // Inicia o DMA com endereços constantes enviando bytes

	char cmd;               // Comando para controle de fluxo do programa
	char cmdAux[2] = {0,};  // Auxiliar para imprimir o comando recebido ao terminal: final '\0'
	uint8_t chan;           // Canal de operação do ADC para printar

	while(1){
	    // Limpa a tela
        uartPrint("\033[2J\r");

        uartPrint("Oscilento: versao 1.6.9\r\n");
        uartPrint("Selecione a entrada: [0:3], 't' p/ todas\r\n");

        while(uartGet(&cmd));
        cmdAux[0] = cmd;
        uartPrint(cmdAux);
        if(cmd == '0' || cmd == '1' || cmd == '2' || cmd == '3' || cmd == 't'){
            chan = cmd - 0x30;
            uartPrint("\r\n\nQual o modo de amostragem?\r\n");
            uartPrint("0: Manual - pressione 'a' p/ amostrar\r\n");
            uartPrint("1: Periódico - 16 amostras por segundo\r\n");
            while(uartGet(&cmd));
            cmdAux[0] = cmd;
            uartPrint(cmdAux);
            if(cmd == '0'){
                // Modo manual
                uartPrint("\r\n\nModo manual selecionado\r\n");
                uartPrint("Pressione 's' a qualquer momento para sair\r\n");
                while(rxBuff[rxWr - 1] != 's'){
                    uartGet(&cmd);
                    if (cmd == 'a'){
                        printStats(chan);
                        cmd = 0;
                    }
                }
            } else if (cmd == '1'){
                // Modo repetitivo
                uartPrint("\r\n\nModo periódico selecionado\r\n");
                uartPrint("Pressione 's' a qualquer momento para sair\r\n");
                while(rxBuff[rxWr-1] != 's'){
                    printStats(chan);
                }
            }
        }
	}

}

// Imprime o resultado de ADC de acordo com o canal de conversão escolhido
void printStats(uint8_t chan){
    uint8_t numBytes;       // Número de bytes escritos pela sprintf
    char msg[50];           // Buffer onde podem ser esritos até 50 caractéres
    switch (chan){
    case 0 ... 3:
        numBytes = sprintf(msg, "0x%03X\r", adcResult[chan]);
        break;
    default:
        numBytes = sprintf(msg, "0x%03X 0x%03X 0x%03X 0x%03X\r", adcResult[0], adcResult[1], adcResult[2], adcResult[3]);
        break;
    }
    printUartDma(msg, numBytes);    // Imprime a mensagem de acordo com o número de bytes escritos a partir do sprintf()
}

// Inicia o ADC com a configuração dada
void initADC(){
    P6SEL |= BIT0 | BIT1 | BIT2 | BIT3;     // Habilita o ADC nos pinos 6.(0-3)

    ADC12CTL0 &= ~ADC12ENC;                 // Desativa o trigger
    REFCTL0 = 0;                            // Desliga o modulo de referencia

    ADC12CTL0 = ADC12SHT0_4      |          // 64 batidas de clock
                ADC12MSC         |          // Multiplas conversıes por trigger
                ADC12ON;                    // Liga o quantizador

    ADC12CTL1 = ADC12CSTARTADD_0 |          // Resultado --> MEM0
                ADC12SHS_1       |          // Trigger: canal 1 do timer A0
                ADC12SHP         |          // Usa o timer interno
                ADC12SSEL_0      |          // Clock de 5Mhz
                ADC12CONSEQ_3;              // Sequence Conv repetitivo

    ADC12CTL2 = ADC12TCOFF       |          // Desliga o sensor de temp
                ADC12RES_1;                 // 10 bits de resolução

    ADC12MCTL0 = 0;                         // P6.0
    ADC12MCTL1 = 1;                         // P6.1
    ADC12MCTL2 = 2;                         // P6.2
    ADC12MCTL3 = 3 | ADC12EOS;              // P6.3

    ADC12IE = BIT3;                         // Interrupção apenas no final da sequência

    TA0CTL = TASSEL__ACLK | MC__UP;         // Configura timer para
    TA0CCR0 = 2048-1;                       // fazer o trigger a 16Hz
    TA0CCR1 = 1;
    TA0CCTL1 = OUTMOD_7;

    ADC12CTL0 |= ADC12ENC;                  // Habilita o conversor

}

// Utiliza o DMA para enviar mensagens por DMA
void printUartDma(char * str, uint8_t nBytes){
    DMA0SA = str;                           // Envia a string
    DMA0DA = &UCA1TXBUF;                    // para txBuf da comunicação uart
    DMA0SZ = nBytes;                        // de tamanho nBytes

    DMA0CTL |= DMAEN;                       // Habilita o envio
}

// Inica o DMA de acordo com a configuração desejada
void initDMA(){
    DMACTL0 = 21;               // Trigger é o UCA1TX flag
    DMA0CTL = DMADT_0       |   // 1 Trigger, 1 cópia
              DMASRCINCR_3  |   // STRING
              DMADSTINCR_0  |   // TXBUF É FIXO
              DMASRCBYTE    |   // Envia 1 byte
              DMADSTBYTE    |
              DMALEVEL;
}


// Inicia a comunicação UART com a counicação desejada
void uartInit(){

    UCA1CTL1 = UCSWRST;             // Reseta interface
    UCA1CTL0 = 0;                   // Usa convenção padrão
    UCA1CTL1 |= UCSSEL__SMCLK;      // clk = 1_048_576  &  BD_rate = 38400
    UCA1BRW = 27;
    UCA1MCTL = UCBRS_2;

    P4SEL |= BIT4 | BIT5;           // Pinos P4.4 e P4.5
    UCA1CTL1 &= ~UCSWRST;           // Libera o RST
    UCA1IE = UCRXIE;                // Habilita a interrupção em RX
    __enable_interrupt();
}

// Envia 1 byte por UART
void uartSend(uint8_t byte){
    while(!(UCA1IFG & UCTXIFG));
    UCA1TXBUF = byte;
}

// Envia uma sequência de caracteres para o UART
void uartPrint(char * str){
    while(*str){
        uartSend(*str++);
    }
}

// Recebe caracteres do buffer de UART
uint8_t uartGet(char * data){
    if (rxWr == rxRd){
        return 1;           // Não ha nada pra leitura
    } else{
        while(rxRd != rxWr){
            *data++ = rxBuff[rxRd];
            rxRd++;
        }
        return 0;
    }
}

// Vetor de interrupção do UART
// Envia bytes recebidos para o buffer
#pragma vector = USCI_A1_VECTOR
__interrupt void UART_ISR(){
    rxBuff[rxWr] = UCA1RXBUF;
    rxWr++;
}

// Vetor de interrupção do ADC
// Salva os resultados dos canais 0 a 3
#pragma vector = ADC12_VECTOR
__interrupt void ADC_ISR(){
    adcResult[0] = ADC12MEM0;
    adcResult[1] = ADC12MEM1;
    adcResult[2] = ADC12MEM2;
    adcResult[3] = ADC12MEM3;
}
