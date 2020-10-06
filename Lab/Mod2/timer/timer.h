/*
 * timer.h
 *
 *  Created on: Oct 2, 2020
 *      Author: ricardosantos
 */

#ifndef LIBS_TIMER_H_
#define LIBS_TIMER_H_

#include <stdint.h>

typedef enum {us, ms, sec} timeUnit_t;

typedef enum {TA_0=0x340, TA_1=0x380, TA_2=0x400, TB_0=0x3C0} timerPin;

typedef enum {CTL=0x00, CCTL=0x02, R=0x10, CCR=0x12, IV=0x2E, EX=0x20} timerRegister;

typedef enum {IFG=0, IE=1, CLR=2, MC=4, ID=6, SEL=8} timerPort;

typedef enum {ACLK=1, SMCLK=2} timerClock;

typedef enum {Stop, UP, Continuous, UP_Down} timerMode;

void wait(uint16_t time, timeUnit_t unit, timerPin timer);


void configTimer(timerPin timer, timerClock clk, timerMode mode);

uint16_t * getTimerPort(timerPin base, timerRegister reg, uint8_t offset);

void setupCTL(uint16_t * timerCTL, uint8_t bit, timerPort port);

void setInterruptionAt(timerPin timer, uint8_t ccr_N, uint16_t time);

void await(timerPin timer, uint8_t cctl_N);

void stopTimer(timerPin timer);

#endif /* LIBS_TIMER_H_ */
