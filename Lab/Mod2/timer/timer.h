/*
 * timer.h
 *
 *  Created on: Oct 2, 2020
 *      Author: ricardosantos
 */

#ifndef LIBS_TIMER_H_
#define LIBS_TIMER_H_

#include <stdint.h>

// enumerador que define os tipos possíveis de tempo para a função wait()
typedef enum {us, ms, sec} timeUnit_t;

// enumerador que define a posição base na memória de cada timer
typedef enum {TA_0=0x340, TA_1=0x380, TA_2=0x400, TB_0=0x3C0} timerPin;

// enumerador que define o offset para cada registrador do timer
typedef enum {CTL=0x00, CCTL=0x02, R=0x10, CCR=0x12, IV=0x2E, EX0=0x20} timerRegister;

// enumerador que deine o bit que será acessado do registrador
// com isso será feita a máscara de set/clear
typedef enum {IFG=0, IE=1, CLR=2, MC=4, ID=6, SEL=8, none=0} timerPort;

// enumerador que define os tipos de clocks do microcontrolador
typedef enum {ACLK=1, SMCLK=2} timerClock;

// enumerador que define os modos de contagem do clock
typedef enum {Stop, UP, Continuous, UP_Down} timerMode;

// Aguarda um determinado tempo em determinado timer
// wait(5, sec, TA_0, 0) :: Aguarda 5 segundos no timer A com CCTL0
void wait(uint16_t time, timeUnit_t unit, timerPin timer);

// Configura um dado timer com o clock, divisor e modo desejado
// configTimer(timerCTL, SMCLK, UP, 0) :: Tx com SM Clock, Modo Up e clock normal
// TimerCTL deve ser encontrado utilizando a função getTimerRegstr()
void configTimer(uint16_t * timerCTL, timerClock clk, timerMode mode, uint8_t div);

// Retona o registrador especificado do timer escolhido
// getTimerRegstr(TA_1, CCR, 2) : TA_1_CCR_2
// getTimerRegstr(TA_1, CTL, 0) : TA_1_CTL
uint16_t * getTimerRegstr(timerPin base, timerRegister reg, uint8_t offset);

// Configura uma porta específica do registrador escolhido
// setupTimerRegister(timerCTL,  <A_clk>, SEL) :: Escolhe o A_CLK para o timer
// timerReg deve ser obtido pela função getTimerRegstr
void setupTimerRegister(uint16_t * timerReg, uint8_t bit, timerPort port);

// Configura uma interrupção na porta CCR específicada
// setInterruptionAt(timerCCR, 200) :: Registra uma interrupção na tempo 20
// timerCCR deve ser obtida por getTimerRegstr
void setInterruptionAt(uint16_t * timerCCR, uint16_t time);

// Trava operações no timer e cctl_N após configurado ccr e ctl
// ccr e ctl podem ser configurado por configTimer() e setInterruption
// ou manualmente
// await(TA_1, 2) :: Aguarda o tempo especificado em ta1ccr2
void await(timerPin timer, uint8_t cctl_N);

// Reinicia e para o timer
// stopTimer(timerCTL) :: zera e para o timer especificado
// timerCTL deve ser obtido por getTimerRegstr()
void stopTimer(uint16_t * timerCTL);

#endif /* LIBS_TIMER_H_ */
