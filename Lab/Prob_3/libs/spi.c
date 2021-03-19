#include <msp430.h>
#include "spi.h"

uint8_t spiTransfer(uint8_t byte){

    while (!(UCB1IFG & UCTXIFG));
    UCB1TXBUF = byte;
    
    while(!(UCB1IFG & UCRXIFG));
    return UCB1RXBUF;
}
