#include "libs/lcd.h"

#define RW BIT1            // Read / Write
#define EN BIT2
#define BT BIT3            // Backlight


uint8_t lcdAddr = 0x3F;

// Configura o lcd
void lcdInit(){
    // Descobre o endereço do lcd
    if(i2cSendByte(lcdAddr, 0))
        lcdAddr = 0x27;

    // Configura o lcd no modo 8 bits
    lcdWriteNibble(0x3, 0);
    lcdWriteNibble(0x3, 0);
    lcdWriteNibble(0x3, 0);

    // Configura o lcd em 4 bits
    lcdWriteNibble(0x2, 0);

    // Configura o cursor e o diplay
    // 0x0F - Cursor piscando
    // 0x0C - Sem cursor
    lcdWriteByte(0x0C, 0);

    lcdWriteByte(0x06, 0);

    // Retorna home e limpa o display
    lcdWriteByte(0x02, 0);
    lcdWriteByte(0x01, 0);
}

void print(char * str){
    while (*str){
        lcdWriteByte(*str++, 1);
    }
}

// Envia um nibble de dados para o lcd
// o envio ncessita que o EN mude de 0 -> 1 -> 0 para que o nibble seja lido
void lcdWriteNibble(uint8_t nibble, uint8_t rs){
                                    // BT   EN   RW  RS
    i2cSendByte(lcdAddr, nibble << 4 | BT | 0  | 0 | rs);
    i2cSendByte(lcdAddr, nibble << 4 | BT | EN | 0 | rs);
    i2cSendByte(lcdAddr, nibble << 4 | BT | 0  | 0 | rs);
//    i2cSendByte(lcdAddr, nibble << 4 | BT | rs);
//    i2cSendByte(lcdAddr, nibble << 4 | BT | EN | rs);
//    i2cSendByte(lcdAddr, nibble << 4 | BT | rs);
}

// Escreve um byte no lcd enviando 2 nibbles
void lcdWriteByte(uint8_t byte, uint8_t rs){
    // Envia os 4 MSB
    lcdWriteNibble(byte >> 4, rs);
    // Envia o 4 LSB
    lcdWriteNibble(byte & 0x0F, rs);
}


void lcdCursor(uint8_t lin, uint8_t col){

    uint8_t pos = (lin*0x40) + col;

    uint8_t fim = 1 << 7 | pos;

    lcdWriteByte(fim, 0);
}

void createCustom(uint8_t cgRamAddr, uint8_t cChar[]){

    // Set CGRAM 0x00 - 0x07
    cgRamAddr *= 0x08;
    uint8_t ramAddr = 1 << 6 | cgRamAddr;
    lcdWriteByte(ramAddr, 0);
    uint8_t i;
    for(i=8; i>0;i--){
        lcdWriteByte(cChar[8-i], 1);
    }

}

void lcdClear(){
    lcdWriteByte(0x01, 0);
}

// 0000_01(I/D)0
void lcdChooseDir(lcdDir dir){
    dir <<= 1;
    uint8_t byte = 1 << 2 | dir;
    lcdWriteByte(byte, 0);
}






















