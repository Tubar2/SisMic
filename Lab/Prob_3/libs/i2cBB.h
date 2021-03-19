#ifndef LIBS_I2CBB_H_
#define LIBS_I2CBB_H_

#include <stdint.h>

#define SDA 2, 5
#define SCL 2, 4        //

#define TCLK4 25, us    // frequencia de 10kHz

/* ------Enviar dados pelo protoolo I2C com Bit Banging------ */
void initI2CBB(uint8_t slaveAddr);
void i2cSendBB(uint8_t addr, uint8_t * data, uint8_t nBytes);
void i2cGetBB(uint8_t addr, uint8_t * data, uint8_t nBytes);


#endif /* */
