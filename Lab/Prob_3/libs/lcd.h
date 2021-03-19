#include <msp430.h>
#include "libs/i2c.h"
#include <stdint.h>

#ifndef _LCD_H
#define _LCD_H

typedef enum {lcdLeft = 0, lcdRight=1} lcdDir;

void lcdInit();

void print(char * str);

void lcdWriteNibble(uint8_t nibble, uint8_t rs);

void lcdWriteByte(uint8_t byte, uint8_t rs);

void lcdCursor(uint8_t lin, uint8_t col);

void createCustom(uint8_t cgRamAddr, uint8_t cChar[]);

void lcdClear();

void lcdChooseDir(lcdDir dir);

#endif //_LCD_H
