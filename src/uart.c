#include "uart.h"

#include <msp430.h>
#include "circular_buffer.h"

CircularBuffer read_buffer;

void uart_init(enum Setting setting) {
    P3SEL |= BIT3 + BIT4;

    UCA0CTL1 |= UCSWRST;                      // **Put state machine in reset**
    UCA0CTL1 |= UCSSEL_2;                     // CLK = SMCLK

    if(setting == SPS30) {
        UCA0BR0 = 8;                              // 115200 (see User's Guide)
        UCA0BR1 = 0;
        UCA0MCTL = UCBRS_6 + UCBRF_0;             // Modulation UCBRSx=6, UCBRFx=0
    } else {
        UCA0BR0 = 0x03;                           // 32kHz/9600=3.41 (see User's Guide)
        UCA0BR1 = 0x00;                           //
        UCA0MCTL = UCBRS_3+UCBRF_0;               // Modulation UCBRSx=3, UCBRFx=0
    }

    UCA0CTL1 &= ~UCSWRST;                     // **Initialize USCI state machine**
    UCA0IE |= UCRXIE;                // Enable USCI_A0 RX interrupt

    cbInit(&read_buffer, 7);

    __bis_SR_register(GIE);
}

int uart_write(unsigned char *buffer, unsigned int buffer_length) {
    int i;
    for(i = 0; i < buffer_length; i++) {
        while(!(UCA0IFG & UCTXIFG));
        UCA0TXBUF = buffer[i];
    }

    return 0;
}

int uart_read(unsigned char *buffer, unsigned int buffer_length) {
    unsigned int i;
    for (i = 0; i < buffer_length; i++) {
        while (cbIsEmpty(&read_buffer));
        cbRead(&read_buffer, &buffer[i]);
    }
    return 0;
}



#pragma vector=USCI_A0_VECTOR
__interrupt void usci_a0_isr() {
    // If it's ready to read
    if (UCA0IFG & UCRXIFG) {
        cbWrite(&read_buffer, UCA0RXBUF);
    }
}
