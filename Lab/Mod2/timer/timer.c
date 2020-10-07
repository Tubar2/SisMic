/*
 * timer.c
 *
 *  Created on: Oct 2, 2020
 *      Author: ricardosantos
 */

#include <msp430.h>
#include "timer.h"


void wait(uint16_t time, timeUnit_t unit, timerPin timer)
{
    const uint8_t regN = 0;     // Define qual o CCRn

    // Endereço do registrador CTL do timer escolhido (TxCTL)
    uint16_t * timerCTL = getTimerRegstr(timer, CTL, 0);
    
    // Endereço do registrador CCR do timer e n escolhidos (TxCCRn)
    uint16_t * timerCCR = getTimerRegstr(timer, CCR, regN);

    switch (unit){

    case us:
        // Configura o timer escolhido (TA0, TA1 ou ...) com SMCLK, MC_UP e InputDiv = 0
        configTimer(timerCTL, SMCLK, UP, 0);

        // Adiciona uma interrução no TxCCRn escolhido no tempo determinado
        setInterruptionAt(timerCCR, time);

        // Aguarda a interrupção acontecer
        await(timer, regN);

        // Para e reinicia o timer
        stopTimer(timerCTL);
        break;

    case ms:
    {
        // Divide o clock de (32MHz por 8) =~ 4 ms
        uint16_t * timerEX0 = getTimerRegstr(timer, EX0, 0);
        setupTimerRegister(timerEX0, 7, none);

        // Configura o timer com ACLK, MC_UP e InputDivider = 4
        configTimer(timerCTL, ACLK, UP, 3);

        // Adiciona uma interrução no TxCCRn escolhido no tempo determinado
        setInterruptionAt(timerCCR, time);

        // Aguarda a interrupção acontecer
        await(timer, regN);

        // Para e reinicia o timer
        stopTimer(timerCTL);
    }
        break;

    case sec:
    {
        // Configura o timer escolhido com ACLK, MC_UP e inputDivider = 0
        configTimer(timerCTL, ACLK, UP, 0);

        // Define variáveis auxiliares
        uint16_t timer_sec = 0;             // Batidas de 1 segundo
        const uint16_t oneSec = 0x8000 - 1; // Um segundo com o clock ACLK

        // Adiciona uma interrupção em 1 segundo
        setInterruptionAt(timerCCR, oneSec);

        // Loop que roda time segundos
        while(timer_sec != time) {

            // Aguarda um segundo
            await(timer, regN);

            // Toda vez que passa um segundo, incrementa o timer de segundos
            timer_sec++;
        }

        stopTimer(timerCTL);
    }
        break;
    }

}

void configTimer(uint16_t * timerCTL, timerClock clk, timerMode mode, uint8_t div){

    // Configura o clock
    setupTimerRegister(timerCTL,  clk, SEL);
    // Configura o modo de contagem
    setupTimerRegister(timerCTL, mode, MC);
    // Configra o divisor de batidas clock
    setupTimerRegister(timerCTL, div, ID);
    // Inicia o clock com 0
    setupTimerRegister(timerCTL, 1, CLR);
}

uint16_t * getTimerRegstr(timerPin base, timerRegister reg, uint8_t offset){
    
    // Forma o endereço do registrador de 16 bits escolhido a partir da
    uint16_t* regstr =
            (uint16_t *) (
                    base    +       // base do timer    eg. TA0 +
                    reg     +       // registrador      eg. SEL +
                    (2*offset)      // offset para os registrador CCTLn/CCRn
            );

    return regstr;
}

void setupTimerRegister(uint16_t * timerReg, uint8_t bit, timerPort port){
    // Máscara dos bits do registrador que serão setados
    uint16_t mask = bit << port;
    *timerReg |= mask;
}

void setInterruptionAt(uint16_t * timerCCR, uint16_t time){
    // Configura CCRn com o tempo especificado, demarcando uma interrupção
    *timerCCR = time - 1;
}

void await(timerPin timer, uint8_t cctl_N) {
    // Pega o endereço da porta CCTL do timer
    uint16_t * timerCCTL = getTimerRegstr(timer, CCTL, cctl_N);

    // Enquanto a flag de interrupção não for setada, trava o programa
    while(!(*timerCCTL & CCIFG));
    
    // Limpa a flag para que novas interrupções possam acontecer
    *timerCCTL &= ~CCIFG;

}

void stopTimer(uint16_t * timerCTL){
    *timerCTL = MC__STOP;
}
