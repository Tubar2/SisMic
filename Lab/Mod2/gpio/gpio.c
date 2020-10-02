/*
 * gpio.c
 *
 *  Created on: Sep 20, 2020
 *      Author: ricardosantos
 */


#include "gpio.h"

// pinMode(1, 0, outPut) irá configurar P1.0 como saída
void pinConfig(uint8_t port, uint8_t bit, enum pinConfigs mode)
{
    uint8_t * pinOut = 0;
    uint8_t * pinRen = 0;
    uint8_t * pinDir = 0;

    // Calcular máscara
    uint8_t mask = 1 << bit;

    // Calcular pino de direção
    pinDir = getPin(port, dir);

    // Calcular pino de saída
    pinOut = getPin(port, out);

    if(mode != output){
        // Calcular pino de Resistência
        pinRen = getPin(port, ren);
    }

    switch (mode) {
    case input:
        setAsInput(mask, pinDir, pinRen, pinOut);
        break;

    case output:
        setAsOutput(mask, pinDir, pinOut);
        break;

    case inPullUp:
        setAsInPUP(mask, pinDir, pinRen, pinOut);
        break;

    case inPullDown:
        setAsInPDwn(mask, pinDir, pinRen, pinOut);
        break;

    default:
        break;
    }
}

uint8_t digitalRead(uint8_t port, uint8_t bit){
    uint8_t mask = 1 << bit;

    uint8_t * pinIn = getPin(port, in);

    return *pinIn & mask;
}

void digitalWrite(uint8_t port, uint8_t bit, uint8_t value) {
    uint8_t mask = 1 << bit;

    uint8_t * pinOut = getPin(port, out);


    if (value){
        *pinOut |= mask;
    } else {
        *pinOut &= ~mask;
    }
}

uint8_t* getPin(uint8_t port, enum pinTypes type){
    uint8_t * pin =
                (uint8_t *) (
                        0x0200                      // base
                        + ((port-1)/2) * 0x20       // base off-set
                        + ((port-1)%2) * 0x01       // IN off-set
                        + type
                );

    return pin;
}

void configInterruptible(uint8_t port, uint8_t bit, enum edgeSelect edge) {
    uint8_t mask = 1 << bit;

    uint8_t * pinIE = getPin(port, ie);
    uint8_t * pinIES = getPin(port, ies);

    // Habilita a porta como possível geardora de interrupções
    setAsIE(mask, pinIE);

    // Configura sob qual transição de sinal será acionada a interrupção
    setAsIES(mask, pinIES, edge);
}

/* Funções Auxiliares */

void setAsInput(uint8_t mask, uint8_t * pinDir, uint8_t * pinRen, uint8_t * pinOut)
{
    *pinOut &= ~mask;// Clear Px. Out
    *pinDir &= ~mask;// Clear Px. Dir
    *pinRen &= ~mask;// Clear Px. Ren
}

void setAsOutput(uint8_t mask, uint8_t * pinDir, uint8_t * pinOut)
{
    *pinOut &= ~mask;// Clear Px. Out
    *pinDir |= mask; // Set Px. Dir
}

void setAsInPUP(uint8_t mask, uint8_t * pinDir, uint8_t * pinRen, uint8_t * pinOut)
{
    *pinDir &= ~mask; // Clear Px. Dir
    *pinRen |= mask;  // Set Px. Ren
    *pinOut |= mask;  // Set Px. Out
}

void setAsInPDwn(uint8_t mask, uint8_t * pinDir, uint8_t * pinRen, uint8_t * pinOut)
{
    *pinDir &= ~mask; // Clear Px. Dir
    *pinRen |= mask;  // Set Px. Ren
    *pinOut &= ~mask; // Clear Px. Out
}

void setAsIE(uint8_t mask, uint8_t * pinIE){
    *pinIE |= mask;   // Set Px. IE
}
void setAsIES(uint8_t mask, uint8_t * pinIES, uint8_t edge){
    if (edge) {
        *pinIES |= mask;  // Set Px.IES (High to low)
    } else {
        *pinIES &= ~mask; // Clear Px.IES (Low to high)
    }
}


