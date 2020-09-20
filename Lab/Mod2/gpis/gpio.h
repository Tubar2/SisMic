
#include <stdint.h>

#ifndef LIBS_GPIO_H_
#define LIBS_GPIO_H_

// enumerador que define tipos de configuração para os pino
enum pinConfigs { input, output, inPullUp, inPullDown };

// enumerador que auxilia no retorno do endereço de uma porta
// como PxIN, PxOUt...
enum pinTypes { in=0x00, out=0x02, dir=0x04, ren=0x06};

// Configura o modo do pino de acordo com o enumerador pinConfigs
// pinConfig(1, 0, input) :: P1.0 como input
void pinConfig(uint8_t port, uint8_t bit, enum pinConfigs mode);

// Retorna o valor de um pino específico
// digitalRead(1, 0) : valor em P1.0
uint8_t digitalRead(uint8_t port, uint8_t bit);

// Set ou clear de um pino específico
// digitalWrite(1, 0, 0) :: Seta P1.0
void digitalWrite(uint8_t port, uint8_t bit, uint8_t value);

// Retorna o endereço da porta de um pino de tipo específico
// getPin(1, in) : P1IN
uint8_t* getPin(uint8_t port, enum pinTypes type);


/* Funções auxiliares */
void setAsInput(uint8_t mask, uint8_t * pinDir, uint8_t * pinRen, uint8_t * pinOut);
void setAsOutput(uint8_t mask, uint8_t * pinDir, uint8_t * pinOut);
void setAsInPUP(uint8_t mask, uint8_t * pinDir, uint8_t * pinRen, uint8_t * pinOut);
void setAsInPDwn(uint8_t mask, uint8_t * pinDir, uint8_t * pinRen, uint8_t * pinOut);


#endif /* LIBS_GPIO_H_ */
