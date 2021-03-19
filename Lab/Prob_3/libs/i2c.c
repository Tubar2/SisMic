#include <msp430.h>
#include "i2c.h"
#include "gpio.h"
#include "timer.h"


/*------USCI--------*/
void i2cInit(){
    // RESETA A INTERFACE
    UCB1CTLW0 = UCSWRST;
    // Configura como mestre com clock em de 1M
    UCB1CTLW0 |= 0X0F00 | UCSSEL__SMCLK;
    // 1M é um clock muito rápido, então dividimos por 100 => clock de 10k
    UCB1BRW = 100;
    // Como estamos usando a interface B1, precisamos olhar no datasheet
    // e setar de acordo
    P4SEL |= (BIT1 | BIT2);
    // Libera o RST
    UCB1CTL1 &= ~UCSWRST;

    // TODO: Implementar interrupções
}

void i2cInitPin(USCI_B usci, uint8_t master, uint8_t addr, uint8_t enableInter){
    // Registradores
    uint16_t * ucbctlw0 = getI2CRegister16(usci, 0);
    uint8_t * ucbbrw = getI2CRegister8(usci, ucb_BRW);
    // Reseta a interface setando o bit reset (0) de ctl1
    *ucbctlw0 |= UCSWRST;
    // Configura como mestre ou escravo
    if(master){
        // 1. Configura como mestre, modo i2c sincrono e clock de 1M
        *ucbctlw0 |= 0X0F00 | UCSSEL__SMCLK;
        // 1M é um clock muito rápido, então dividimos por 100 => clock de 10k
        *ucbbrw = 100;
    }
    else {
        uint8_t * ucbOA = getI2CRegister8(usci, ucb_OA);
        // Configurar como escravo
        *ucbctlw0 |= 0x700;
        *ucbOA = addr;
    }

    // Configura o pino de acordo com o ucbx
    configPinI2C(usci, master);

    // Libera o RST
    *ucbctlw0 &= ~UCSWRST;

    // Configura interrupções
    if (enableInter){
        uint8_t * ucbIE = getI2CRegister8(usci, ucb_IE);
        // Habilita interrupções na transmissão e recepção
        // Os bits setados são escolhido pela própria entrada
        // 1 -> UCRXIE, 2 -> UCTXIE, 3 -> UCRXIE e UCTXIE...
        *ucbIE = enableInter;
    }

}

uint8_t i2cSend(uint8_t addr, uint8_t * data, uint8_t nBytes){
    // Endereço aonde será enviado o dado (SA = Slave Address)
    UCB1I2CSA = addr;
    // Transmite o endereço
    UCB1CTLW0 |= UCTXSTT |      // Gera condição de START
                 UCTR;           // Modo transmissor
    // Espera o START ocorer
    while(!(UCB1IFG & UCTXIFG));// Espera o buffer de transmissão ficar vazio
    UCB1TXBUF = *data++;        // Escreve um byte no buffer
    nBytes--;

    while(UCB1CTLW0 & UCTXSTT); // Aguarda ACK/NACK
    // Em um NACK:
    if(UCB1IFG & UCNACKIFG){
        UCB1CTLW0 |= UCTXSTP;   // Envia um stop
        while(UCB1CTLW0 & UCTXSTP); // Espera ser efetivamente parado
        return 1;                   // 1 == ERRO
    }
    // Caso não houve NACK, continuar transmitindo bytes
    while (nBytes--){
        while(!(UCB1IFG & UCTXIFG));// Espera o buffer de transmissão ficar vazio
        UCB1TXBUF = *data++;        // Escreve um byte no buffer
    }
    while(!(UCB1IFG & UCTXIFG));    // Espera enviar o último byte
    UCB1CTLW0 |= UCTXSTP;           // Envia um STOP
    
    while(UCB1CTLW0 & UCTXSTP);     // Espera enviar o STOP

    return 0;                       // 0 == SUCESSO
}

uint8_t i2cSendPin(USCI_B usci, uint8_t addr, uint8_t * data, uint8_t nBytes){
    // Registradores
    uint16_t * ucbSA = getI2CRegister16(usci, ucb_SA);
    uint16_t * ucbctlw0 = getI2CRegister16(usci, 0);

    // 1. Endereço em CSA e em CTLW0 start no modo transmissor
    *ucbSA = addr;
    *ucbctlw0 |= UCTXSTT |      // Gera condição de START
                 UCTR;           // Modo transmissor

    // Registradores
    uint8_t * ucbIFG = getI2CRegister8(usci, ucb_IFG);
    uint8_t * ucbTXBUF = getI2CRegister8(usci, ucb_TXBUF);
    // 2. Faz a comunicação
    while(!((*ucbIFG) & UCTXIFG));// Espera o buffer de transmissão ficar vazio
    *ucbTXBUF = *data++;          // Escreve um byte no buffer
    nBytes--;

    while((*ucbctlw0) & UCTXSTT);   // Aguarda ACK/NACK
    // Em um NACK:
    if((*ucbIFG) & UCNACKIFG){
        *ucbctlw0 |= UCTXSTP;   // Envia um stop
        while((*ucbctlw0) & UCTXSTP); // Espera ser efetivamente parado
        return 1;                   // 1 == ERRO
    }
    // Caso não houve NACK, continuar transmitindo bytes
    while (nBytes--){
        while(!((*ucbIFG) & UCTXIFG));// Espera o buffer de transmissão ficar vazio
        *ucbTXBUF = *data++;        // Escreve um byte no buffer
    }
    while(!((*ucbIFG) & UCTXIFG));    // Espera enviar o último byte
    *ucbctlw0 |= UCTXSTP;           // Envia um STOP

    while((*ucbctlw0) & UCTXSTP);     // Espera enviar o STOP

    return 0;
}

uint8_t i2cSendByte(uint8_t addr, uint8_t byte){
    return i2cSend(addr, &byte, 1);
}

// TODO: Implement
uint8_t i2cGet(uint8_t addr, uint8_t * data, uint8_t nBytes){
    // Endereço aonde sera lido o dado
    UCB1I2CSA = addr;
    // Transmite o endereço
    UCB1CTLW0 |= UCTXSTT;           // Gera condição de START no modo receptor

    // Espera o START ocorer
    while(!(UCB1IFG & UCRXIFG));// Espera o buffer de recepção ficar vazio
    *data++ = UCB1RXBUF;        // Lê um byte do buffer em data
    nBytes--;

    while(UCB1CTLW0 & UCTXSTT); // Aguarda ACK/NACK
    // Em um NACK:
    if(UCB1IFG & UCNACKIFG){
        UCB1CTLW0 |= UCTXSTP;   // Envia um stop
        while(UCB1CTLW0 & UCTXSTP); // Espera ser efetivamente parado
        return 1;                   // 1 == ERRO
    }
    // Caso não houve NACK, continuar lendo bytes
    while (nBytes--){
        while(!(UCB1IFG & UCRXIFG));// Espera o buffer de recepção ficar vazio
        *data++ = UCB1RXBUF;        // Lê um byte do buffer em data
    }
    while(!(UCB1IFG & UCRXIFG));    // Espera ler o último byte
    UCB1CTLW0 |= UCTXSTP;           // Envia um STOP

    while(UCB1CTLW0 & UCTXSTP);     // Espera enviar o STOP

    return 0;                       // 0 == SUCESSO
}

uint8_t i2cGetPin(USCI_B usci, uint8_t addr, uint8_t * data, uint8_t nBytes){
    // Registradores
    uint8_t * ucbSA = getI2CRegister8(usci, ucb_SA);
    uint16_t * ucbctlw0 = getI2CRegister16(usci, 0);
    // Endereço aonde sera lido o dado
    *ucbSA = addr;
    // Transmite o endereço
    *ucbctlw0 |= UCTXSTT;           // Gera condição de START no modo receptor

    // Registradores
    uint8_t * ucbIFG = getI2CRegister8(usci, ucb_IFG);
    uint8_t * ucbRXBUF = getI2CRegister8(usci, ucb_RXBUF);


    while( (*ucbctlw0) & UCTXSTT); // Aguarda ACK/NACK
    // Em um NACK:
    if((*ucbIFG) & UCNACKIFG){
        *ucbctlw0 |= UCTXSTP;   // Envia um stop
        while((*ucbctlw0) & UCTXSTP); // Espera ser efetivamente parado
        return 1;                   // 1 == ERRO
    }
    // Caso não houve NACK, continuar lendo bytes
    while (nBytes--){
        while(!(*ucbIFG & UCRXIFG));// Espera o buffer de recepção receber dados
        *data++ = *ucbRXBUF;        // Lê um byte do buffer em data
    }
    while(!((*ucbIFG) & UCRXIFG));    // Espera ler o último byte
    *ucbctlw0 |= UCTXSTP;           // Envia um STOP

    while(*ucbctlw0 & UCTXSTP);     // Espera enviar o STOP

    return 0;                       // 0 == SUCESSO
}

uint8_t i2cGetByte(uint8_t addr, uint8_t * byte){
    return i2cGet(addr, byte, 1);
}

/* -------Auxiliares-------- */
uint16_t * getI2CRegister16(USCI_B base, USCI_B_Registers reg){
    return (uint16_t *) (base + reg);
}

uint8_t * getI2CRegister8(USCI_B base, USCI_B_Registers reg){
    return (uint8_t *) (base + reg);
}

void configPinI2C(USCI_B usci, uint8_t master){
    // Colocar resistor de pull-up??? PxREN e PxOUT |= 1
    switch (usci){
    case usciB0:
        P3SEL |= (BIT0 | BIT1);
        P3REN |= (BIT0 | BIT1);
        P3OUT |= (BIT0 | BIT1);
        if (master){

        } else {
            P3DIR &= ~(BIT0 | BIT1);
        }
        break;

    case usciB1:
        P4SEL |= (BIT1 | BIT2);
        P4REN |= (BIT1 | BIT2);
        P4OUT |= (BIT1 | BIT2);
        if (master){

        } else {
            P4DIR &= ~(BIT1 | BIT2);
        }
    break;

    default:
        break;
    }

}
