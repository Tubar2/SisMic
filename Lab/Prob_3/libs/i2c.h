/*
 * i2c.h
 *
 *  Created on: Oct 18, 2020
 *      Author: ricardosantos
 */

#ifndef LIBS_I2C_H_
#define LIBS_I2C_H_

#include <stdint.h>


typedef enum {usciB0=0x05E0, usciB1=0x0620} USCI_B;
typedef enum {ucb_CTL1=0X0, ucb_CTL0=0X01, ucb_BRW=0x06, 
    ucb_RXBUF=0x0C, ucb_TXBUF=0x0E,
    ucb_OA=0x10, ucb_SA=0x12, ucb_IE=0x1C,
    ucb_IFG=0x1D} USCI_B_Registers;


/* ------Enviar dados pelo protocolo I2C pelo protocolo dedicado------ */
void i2cInit();
void i2cInitPin(USCI_B usci, uint8_t master, uint8_t addr, uint8_t enableInter);

// Envia 'nBytes' para o endereço 'addr' tirados de 'data'
// Retorna 0 em sucesso
uint8_t i2cSend(uint8_t addr, uint8_t * data, uint8_t nBytes);

uint8_t i2cSendPin(USCI_B usci, uint8_t addr, uint8_t * data, uint8_t nBytes);

// Lê 'nBytes' do endereço 'addr' e escreve em 'data'
// Retorna 0 em sucesso
uint8_t i2cGet(uint8_t addr, uint8_t * data, uint8_t bNytes);

uint8_t i2cGetPin(USCI_B usci, uint8_t addr, uint8_t * data, uint8_t nBytes);

// Envia o dado 'byte' para o endereço 'addr'
// Retorna 0 em sucesso
uint8_t i2cSendByte(uint8_t addr, uint8_t byte);
// Lê um dado em 'byte' do endereço 'addr'
// Retorna 0 em sucesso
uint8_t i2cGetByte(uint8_t addr, uint8_t * byte);

/* Auxiliares */
uint8_t * getI2CRegister8(USCI_B base, USCI_B_Registers reg);

uint16_t * getI2CRegister16(USCI_B base, USCI_B_Registers reg);

void configPinI2C(USCI_B usci, uint8_t master);

#endif /* LIBS_I2C_H_ */
