#include <msp430.h>
#include "i2cBB.h"
#include "gpio.h"
#include "timer.h"


/*------Bit Bang--------*/
/* Camada Física */
void initI2CBB(uint8_t slaveAddr){
	// Setar o bit de RST
	UCB1CTL1 = UCSWRST;

	// Configurar o escravo
	UCB1CTL0 =
			UCMODE_3 |		// MODO I2C
			UCSYNC;			// Modo síncrono
	
	// Configurar o endereço local de handle do escravo
	UCB1I2COA = UCGCEN | slaveAddr;

	// Configurar os pinos para I2C e não GPIO
	P4SEL |= (BIT1 | BIT2);
	P4DIR &= ~(BIT1 | BIT2);
	P4REN |= ~(BIT1 | BIT2);	// Usa o resistor de pull-up
	P4OUT |= (BIT1 | BIT2);

	// Zera o bit de RST
	UCB1CTL1 &= ~UCSWRST;

	// Habilitar interrupções
	UCB1IE = UCRXIE | 			// Interrupção gerada ao receber dados e buffer cheio
			 UCTXIE;			// Interrupção ao receber dados

}

// "Soltar" a linha efetivamento enviando 1 devido ao resistor de PullUp
// Pino é escolhido por P(port).pin
void i2cRelease(uint8_t port, uint8_t pin){
    // Botar o pino em dreno aberto
    pinConfig(port, pin, inPullUp);
}
// "Soltar" a linha
// Pino é escolhido a partir da struct Pin
void i2cReleasePin(Pin * pin){
    // Botar o pino em dreno aberto
    pinConfigPin(pin, inPullUp);
}

// Escrever 0
// Pino é escolhido por P(port).pin
void i2cPullLow(uint8_t port, uint8_t pin) {
    digitalWrite(port, pin, 0);
    pinConfig(port, pin, output);
}
// Escrever 0
// Pino é escolhido pela struct Pin
void i2cPullLowPin(Pin * pin) {
    digitalWrite(pin->port, pin->bit, 0);
    pinConfigPin(pin, output);
}

/* Ações de 1 bit */

// Inicia o clock
// Obs: Utiliza o TA0 para aguardar 100 us
void i2cStart(){
    wait(TCLK4, TA_0);         // Espera 1/4 do clock em TA0
    wait(TCLK4, TA_0);         // Espera 1/4 do clock em TA0
    i2cPullLow(SDA);
    wait(TCLK4, TA_0);         // Espera 1/4 do clock em TA0
    wait(TCLK4, TA_0);         // Espera 1/4 do clock em TA0
}

// Inicia o clock
// Obs: Utiliza o TA0 para aguardar 100 us
void i2cStop(){
    wait(TCLK4, TA_0);         // Espera 1/4 do clock em TA0
    wait(TCLK4, TA_0);         // Espera 1/4 do clock em TA0
    i2cRelease(SDA);
    wait(TCLK4, TA_0);         // Espera 1/4 do clock em TA0
    wait(TCLK4, TA_0);         // Espera 1/4 do clock em TA0
}

void i2cSendBit(uint8_t bitValue){
    if(bitValue){
        i2cRelease(SDA);
    } else {
        i2cPullLow(SDA);
    }
    wait(TCLK4, TA_0);         // Espera 1/4 do clock em TA0

    i2cRelease(SCL);

    wait(TCLK4, TA_0);         // Espera 1/4 do clock em TA0
    wait(TCLK4, TA_0);         // Espera 1/4 do clock em TA0

    i2cPullLow(SCL);

    wait(TCLK4, TA_0);         // Espera 1/4 do clock em TA0
}

uint8_t i2cGetBit(){
    uint8_t sample;

    i2cRelease(SDA);           // Libera SD para o escravo poder escrever

    wait(TCLK4, TA_0);         // Espera 1/4 do clock em TA0
    i2cRelease(SCL);
    wait(TCLK4, TA_0);         // Espera 1/4 do clock em TA0

    sample = digitalRead(SDA);

    wait(TCLK4, TA_0);         // Espera 1/4 do clock em TA0

    i2cPullLow(SCL);

    return sample;
}

/* Ações multibit */

uint8_t i2cSendByteBB(uint8_t byte) {
    uint8_t mask = 0x80;            // 1000 0000b
    while (mask){
        i2cSendBit(byte & mask);    // Envia do bit mais significativo para
        mask >>= 1;                 // o menos
    }
    
    return i2cGetBit();             // Ciclo de Acknowledge
}

uint8_t i2cGetByteBB(uint8_t lastByte){
    uint8_t mask = 0x80;            // 1000 0000b
    uint8_t receivedByte = 0;

    while (mask){
        if(i2cGetBit()){
            receivedByte |= mask;
        }                
        mask >>= 1;
    }
    
    i2cSendBit(lastByte);             // Ciclo de Acknowledge

    return receivedByte;
}

/* Protocolo Completo */

void i2cSendBB(uint8_t addr, uint8_t * data, uint8_t nBytes){
    i2cStart();

    // Se o escravo não responder, não enviar dados
    if (i2cSendByteBB(addr << 1 | 0)){  // Bit Endereço + WRITE
        i2cStop();
        return;
    }

    while (nBytes--){
        i2cSendByteBB(*data++);
    }
    i2cStop();
}

void i2cGetBB(uint8_t addr, uint8_t * data, uint8_t nBytes){
    i2cStart();

    // Se o escravo não responder, não enviar dados
    if (i2cSendByteBB(addr << 1 | 1)){  // Bit Endereço + READ
        i2cStop();
        return;
    }

    while (nBytes--){
        *data++ = i2cGetByteBB(!nBytes);
    }
    i2cStop();
}
/*----------------------------------------*/
