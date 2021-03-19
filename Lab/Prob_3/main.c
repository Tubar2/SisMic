#include <msp430.h>
#include "libs/i2c.h"
#include "libs/lcd.h"
#include "libs/timer.h"
#include "libs/gpio.h"

/**
 * main.c
 */

uint16_t x[8], y[8];
volatile uint8_t flag;
uint8_t i, mode;
char msg[2][17];

void showBlocks(uint16_t vM);
void calcMedia();
void displayLCD(uint16_t xM, uint16_t yM);
uint16_t media(uint16_t arr[8]);
void ocupyMem(uint16_t xM, uint8_t space);
void specialCharSetup();
void displayBlocks(uint16_t xM, uint16_t yM);
void showBlocks2(uint16_t vM, uint8_t lr);

void initADC(){
    P6SEL |= (BIT0 | BIT1);             // Leitura ADC nos pinos 6.0 e 6.1
    P2DIR &= ~BIT4;                     // P2.4 -> Switch do joystick
    P2REN |= BIT4;                      // com resistor de pull-up
    P2OUT |= BIT4;
    P2IE  |= BIT4;                      // Interrupção no flanco de descida
    P2IES |= BIT4;                      // no switch do joystick
    P2IFG &= ~BIT4;

    ADC12CTL0 &= ~ADC12ENC;             // Desativa o trigger
    REFCTL0 = 0;                        // Desliga módulo de referência
    ADC12CTL0 = ADC12SHT0_2         |   // 16 batidas de clock
                ADC12MSC            |   // Multiplas conversões
                ADC12ON;                // Liga o quantizador

    ADC12CTL1 = ADC12CSTARTADD_0    |   // MEM0
                ADC12SHS_1          |   // Trigger: TA0.1
                ADC12SSEL_0         |   // Clock 5MHz
                ADC12CONSEQ_3;          // Mult conv REPETITIVO

    ADC12CTL2 = ADC12TCOFF          |   // Desliga sensor temp
                ADC12RES_2;             // 12 bits resolução

    // Faz as leituras do ADC nas memórias 0 e 1
    ADC12MCTL0 = 0;
    ADC12MCTL1 = 1 | ADC12EOS;

    ADC12IE = BIT1;                     // Habilita Interrupções NO FINAL DA SEQ
}

int main(void){
    WDTCTL = WDTPW | WDTHOLD;

    initADC();                          // Inicia o ADC em p6.0 e p6.1

    __enable_interrupt();               // Habilita interrupções

    i2cInit();                          // Inicia o módulo interno de i2c nos pinos
                                        // p4.1 (SDA) e p4.2 (SCL)

    lcdInit();                          // Inicia o lcd com tela limpa, cursor
                                        // invisivel e modo 4 bits MSB-first
    specialCharSetup();                 // Cria os caracteres especiais na memória do LCD

    TA0CTL = TASSEL__ACLK | MC__UP;     // Configura TA0 para controloar a leitura
    TA0CCR0 = 2048;                     // do ADC em 12Hz
    TA0CCR1 = 1;

    TA0CCTL1 = OUTMOD_7;

    TA1CTL = TASSEL__ACLK | MC__STOP | TACLR;   // Configura TA1 para controlar
    TA1CCR0 = 0X1000;                           // o rebote do switch do joystick
    TA1CCTL0 = CCIE;

    Pin red = {1,0};                    // Pino do LED vermelho como saída
    pinConfigPin(&red, output);

    ADC12CTL0 |= ADC12ENC;              // Habilita o ADC

    uint16_t xM, yM;                    // xMédio e yMédio
    mode = 0;                           // Inicia no modo 0
    while (1){
                                        // Caso tenham sido feita as 8 leituras
        if(flag){                       // a interrupção no adc habilita essa flag
            calcMedia(&xM, &yM);        // Calcula a média das 8 leituras em x e y
            displayLCD(xM, yM);         // Mostra as leituras no lcd, de acordo com modo

            flag = 0;                   // Reinicia a flag para 0


        }

    }
}

// Calcula a média de 8 medidas
uint16_t media(uint16_t arr[8]){
    uint32_t aux = 0;
    uint8_t j;
    for(j = 8; j > 0; j--){
        aux += arr[8-j];
    }
    return aux/8;
}

// Preenche o buffer da msg de acordo com o modo
void displayLCD(uint16_t xM, uint16_t yM){
    lcdClear();
    switch(mode){
    case 0:
        msg[0][0] = 'A';
        msg[0][1] = '0';
        msg[0][2] = ':';
        msg[0][3] = ' ';
        ocupyMem(xM, 0);
        msg[0][9] = 'V';
        msg[0][10] = ' ';
        msg[0][11] = ' ';
        msg[0][12] = '0';

        msg[1][0] = 'A';
        msg[1][1] = '1';
        msg[1][2] = ':';
        msg[1][3] = ' ';
        ocupyMem(yM, 1);
        msg[1][9] = 'V';
        msg[1][10] = ' ';
        msg[1][11] = ' ';
        msg[1][12] = '0';

        lcdChooseDir(lcdRight);
        lcdCursor(0, 0);
        print(msg[0]);
        lcdCursor(1, 0);
        print(msg[1]);
        break;
    case 1:
        msg[0][0] = 'A';
        msg[0][1] = '0';
        msg[0][2] = ':';
        msg[0][3] = ' ';
        ocupyMem(xM, 0);
        msg[0][9] = 'V';
        msg[0][10] = ' ';
        msg[0][11] = ' ';
        msg[0][12] = '0';

        lcdChooseDir(lcdRight);
        lcdCursor(0, 0);
        print(msg[0]);
        lcdCursor(1, 0);
        showBlocks(xM);
        break;
    case 2:
        msg[1][0] = 'A';
        msg[1][1] = '1';
        msg[1][2] = ':';
        msg[1][3] = ' ';
        ocupyMem(yM, 1);
        msg[1][9] = 'V';
        msg[1][10] = ' ';
        msg[1][11] = ' ';
        msg[1][12] = '0';

        lcdChooseDir(lcdRight);
        lcdCursor(0, 0);
        print(msg[1]);
        lcdCursor(1, 0);
        showBlocks(yM);

        break;
    case 3:
        displayBlocks(xM, yM);
        break;
    default:
        break;
    }
}

// Mostra os blocos no LCD no modo 3
void displayBlocks(uint16_t xM, uint16_t yM){
    if(xM > 0x7FD){
        lcdCursor(0, 8);
        lcdChooseDir(lcdRight);
        showBlocks2(xM-0x7FD, 0);

    } else {
        lcdCursor(0, 7);
        lcdChooseDir(lcdLeft);
        showBlocks2(0x7FD-xM, 4);
    }
    if (yM > 0x7FC){
        lcdCursor(1, 8);
        lcdChooseDir(lcdRight);
        showBlocks2(yM-0x7FD, 0);
    } else {
        lcdCursor(1, 7);
        lcdChooseDir(lcdLeft);
        showBlocks2(0x7FD-yM, 4);
    }
}

// Função auxilia o desplay dos blocos no modo 3
void showBlocks2(uint16_t vM, uint8_t lr){
    uint8_t val, val2, numCols;
    val = vM / 255;
    val2 = vM % 255;
    numCols = val;
    while(numCols--){
        lcdWriteByte(0xFF, 1);
    }
    if (val2){
        uint8_t lastCol = val2 / 5;
        if(lastCol < 10){
            lcdWriteByte(0+lr, 1);
        } else if(lastCol < 20){
            lcdWriteByte(1+lr, 1);
        } else if(lastCol < 30){
            lcdWriteByte(2+lr, 1);
        } else if (lastCol < 40){
            lcdWriteByte(3+lr, 1);
        } else {
            lcdWriteByte(0xFF, 1);
        }
    }
}

// Mostra os blocos no modo 0-2
void showBlocks(uint16_t vM){
    uint8_t val, val2, numCols;
    val = vM / 255;
    val2 = vM % 255;
    numCols = val;
    while(numCols--){
        lcdWriteByte(0xFF, 1);
    }
    if (val2){
        uint8_t lastCol = val2 / 5;
        if(lastCol < 10){
            lcdWriteByte(0, 1);
        } else if(lastCol < 20){
            lcdWriteByte(1, 1);
        } else if(lastCol < 30){
            lcdWriteByte(2, 1);
        } else if (lastCol < 40){
            lcdWriteByte(3, 1);
        } else {
            lcdWriteByte(0xFF, 1);
        }
    }
}

// Preenche os espaços de voltagem e hexadecimal do buffer de mensagem
void ocupyMem(uint16_t vM, uint8_t space){
    uint16_t val = (vM * 10) / 12.41;
    uint16_t valAux = val;
    uint16_t power = 1000;
    uint8_t aux;
    for(aux = 4; aux < 9; aux++){
        if(aux == 5){
            msg[space][aux] = ',';
            continue;
        }
        uint8_t numI = valAux / power;
        msg[space][aux] = numI+48;
        valAux = valAux % power;
        if(power!=1)
            power/=10;
    }

    uint8_t n;
    uint8_t div1 = 8;
    uint16_t div2 = 0x0F00;
    for(aux=13; aux<16; aux++){
        if(div1 == 2)
            div1 = 0;
        n = ((vM & div2) >> div1);
        if(n < 10)
            msg[space][aux] = n+0x30;
        else
            msg[space][aux] = n+0x37;
        div2 >>= 4;
        div1 >>= 1;
    }
}

// Calcula a media de x e y de acordo com o modo
void calcMedia(uint16_t *xM, uint16_t *yM){
    switch (mode){
    case 3:
    case 0:
        *xM = media(x);
        *yM = media(y);
        break;
    case 1:
        *xM = media(x);
        break;
    case 2:
        *yM = media(y);
        break;
    default:
        break;
    }
}

// Preenche a memória do LCD com os caracteres especiais
void specialCharSetup(){
    uint8_t cChar0[] = {
                        0x10,
                        0x10,
                        0x10,
                        0x10,
                        0x10,
                        0x10,
                        0x10,
                        0x10
    };
    createCustom(0, cChar0);
    uint8_t cChar1[] = {
                        0x18,
                        0x18,
                        0x18,
                        0x18,
                        0x18,
                        0x18,
                        0x18,
                        0x18
        };
        createCustom(1, cChar1);
        uint8_t cChar2[] = {
                        0x1C,
                        0x1C,
                        0x1C,
                        0x1C,
                        0x1C,
                        0x1C,
                        0x1C,
                        0x1C
        };
        createCustom(2, cChar2);
        uint8_t cChar3[] = {
                        0x1E,
                        0x1E,
                        0x1E,
                        0x1E,
                        0x1E,
                        0x1E,
                        0x1E,
                        0x1E
        };
        createCustom(3, cChar3);
        uint8_t cChar4[] = {
                        0x1,
                        0x1,
                        0x1,
                        0x1,
                        0x1,
                        0x1,
                        0x1,
                        0x1
        };
        createCustom(4, cChar4);
        uint8_t cChar5[] = {
                        0x3,
                        0x3,
                        0x3,
                        0x3,
                        0x3,
                        0x3,
                        0x3,
                        0x3
        };
        createCustom(5, cChar5);
        uint8_t cChar6[] = {
                        0x7,
                        0x7,
                        0x7,
                        0x7,
                        0x7,
                        0x7,
                        0x7,
                        0x7
        };
        createCustom(6, cChar6);
        uint8_t cChar7[] = {
                        0xF,
                        0xF,
                        0xF,
                        0xF,
                        0xF,
                        0xF,
                        0xF,
                        0xF
        };
        createCustom(7, cChar7);

}

// Funcão de interrupção para o conversor AD
#pragma vector = ADC12_VECTOR
__interrupt void ADC_ISR(){
    x[i] = ADC12MEM0;
    y[i] = ADC12MEM1;

    i++;
    if( i == 8){
        i = 0;
        flag = 1;
    }
}

// Função de interrupção para o pino
#pragma vector = PORT2_VECTOR
__interrupt void SW_ISR(){
    switch(P2IV){
    case 0x0A:
        P1OUT ^= BIT0;
        P2IE &= ~BIT4;
        TA1CTL |= MC__UP | TACLR;

        if(P2IES & BIT4){
            mode++;
            if(mode == 4){
                mode = 0;
            }
        }
        P2IES ^= BIT4;
        break;
    default:
        break;
    }
}

// Função de interrupção no timer para controlar o debounce no switch
#pragma vector = TIMER1_A0_VECTOR
__interrupt void TA1_CCR0_ISR(){
    P2IE |= BIT4;

    TA1CTL &= ~MC__UP;

    P2IFG &= ~BIT4;
}
















