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

    const uint8_t regN = 0;

    switch (unit){

    case us:
        // Configura o timer (TA0 ou TA1 ou ...) com SMCLK e MC_UP
        configTimer(timer, SMCLK, UP);

        // Adiciona uma interrução em no timer no tempo especificado
        setInterruptionAt(timer, regN, time);

        // Aguarda a interrupção ser tratada
        await(timer, regN);

        // Para o timer
        stopTimer(timer);
        break;

    case ms:
        configTimer(timer, ACLK, UP);

        time = (time*100) / 3;

        setInterruptionAt(timer, regN, time);

        await(timer, regN);

        stopTimer(timer);
        break;

    case sec:
    {
        configTimer(timer, ACLK, UP);

        uint16_t timer_sec = 0;
        const uint16_t oneSec = 0x8000 - 1;

        setInterruptionAt(timer, regN, oneSec);

        while(timer_sec != time) {

            await(timer, regN);

            timer_sec++;
        }

        stopTimer(timer);
    }
        break;
    }

}

void configTimer(timerPin timer, timerClock clk, timerMode mode){
    uint16_t * timerCTL = getTimerPort(timer, CTL, 0);

    setupCTL(timerCTL,  clk, SEL);
    setupCTL(timerCTL, mode, MC);
    setupCTL(timerCTL, 1, CLR);
}

uint16_t * getTimerPort(timerPin base, timerRegister reg, uint8_t offset){
    uint16_t* pin =
            (uint16_t *) (
                    base    +
                    reg     +
                    (2*offset)
            );

    return pin;
}

void setupCTL(uint16_t * timerCTL, uint8_t bit, timerPort port){
    uint16_t mask = bit << port;
    *timerCTL |= mask;
}

void setInterruptionAt(timerPin timer, uint8_t ccr_N, uint16_t time){
    uint16_t * timerCCR = getTimerPort(timer, CCR, ccr_N);

    *timerCCR = time - 1;
}

void await(timerPin timer, uint8_t cctl_N) {
    uint16_t * timerCCTL = getTimerPort(timer, CCTL, cctl_N);

    while(!(*timerCCTL & CCIFG));
    *timerCCTL &= ~CCIFG;

}

void stopTimer(timerPin timer){
    uint16_t * timerCTL = getTimerPort(timer, CTL, 0);

    *timerCTL = MC__STOP;
}
