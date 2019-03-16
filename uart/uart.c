#include "uart.h"

#include <msp430.h>
#include "circular_buffer.h"

circular_buffer write_buffer;
circular_buffer read_buffer;

void uart_init() {
    P3SEL |= BIT3 + BIT4;

    UCA0CTL1 |= UCSWRST;                      // **Put state machine in reset**
    UCA0CTL1 |= UCSSEL_1;                     // CLK = ACLK
    UCA0BR0 = 0x03;                           // 32kHz/9600=3.41 (see User's Guide)
    UCA0BR1 = 0x00;                           //
    UCA0MCTL = UCBRS_3+UCBRF_0;               // Modulation UCBRSx=3, UCBRFx=0
    UCA0CTL1 &= ~UCSWRST;                     // **Initialize USCI state machine**
    UCA0IE |= UCRXIE;                         // Enable USCI_A0 RX interrupt

    __bis_SR_register(GIE);

    circular_buffer_init(&write_buffer);
    circular_buffer_init(&read_buffer);
}

int uart_write(unsigned char *buffer, unsigned int buffer_length) {
    unsigned int i;
    for (i = 0; i < buffer_length; i++) {
        circular_buffer_write(&write_buffer, buffer[i]);
    }
    return 0;
}

int uart_read(unsigned char *buffer, unsigned int buffer_length) {
    unsigned int i;
    for (i = 0; i < buffer_length; i++) {
        while (circular_buffer_is_empty(&read_buffer));
        circular_buffer_read(&read_buffer, &buffer[i]);
    }
    return 0;
}

int uart_read_until(unsigned char *sample, unsigned int sample_length, unsigned char *buffer, unsigned int buffer_length) {
    unsigned int sample_i = 0;

    unsigned int buffer_i = 0;
    unsigned char fallback = 0;

    // If buffer isn't defined, points to 1 byte, so "buffer_length" is 1
    if (!buffer) {
        buffer = &fallback;
        buffer_length = 1;
    }

    while (1) {
        uart_read(&buffer[buffer_i], 1);

        if (buffer[buffer_i] == sample[sample_i]) {
            sample_i++;
        } else if (sample_i > 0) {
            sample_i = 0;
        }

        if (sample_i == sample_length) {
            break;
        }

        buffer_i = (buffer_i + 1) % buffer_length;
    }
    return 0;
}

#pragma vector=USCI_A0_VECTOR
__interrupt void usci_a0_isr() {
    // If it's ready to write
    if (UCA0IFG & UCTXIFG) {
        unsigned char tmp;
        circular_buffer_read(&write_buffer, &tmp);
        UCA0TXBUF = tmp;
    }

    // If it's ready to read
    if (UCA0IFG & UCRXIFG) {
        circular_buffer_write(&read_buffer, UCA0RXBUF);
    }
}
