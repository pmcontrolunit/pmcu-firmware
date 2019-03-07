#include "uart.h"
#include <msp430.h>

using namespace Uart;

CircularBuffer write_buffer(256);
CircularBuffer read_buffer(256);

void Uart::init() {
    P3SEL |= BIT3 + BIT4;

    UCA0CTL1 |= UCSWRST;                      // **Put state machine in reset**
    UCA0CTL1 |= UCSSEL_1;                     // CLK = ACLK
    UCA0BR0 = 0x03;                           // 32kHz/9600=3.41 (see User's Guide)
    UCA0BR1 = 0x00;                           //
    UCA0MCTL = UCBRS_3+UCBRF_0;               // Modulation UCBRSx=3, UCBRFx=0
    UCA0CTL1 &= ~UCSWRST;                     // **Initialize USCI state machine**
    UCA0IE |= UCRXIE;                         // Enable USCI_A0 RX interrupt

    __bis_SR_register(GIE);
}

bool Uart::write(unsigned char *buffer, unsigned int length) {
    if (length > write_buffer.get_size() - write_buffer.get_count()) {
        return false;
    }
    for (int i = length - 1; i >= 0; i--) {
        write_buffer.write(buffer[i]);
    }
    return true;
}

void Uart::read(unsigned char *buffer, unsigned int length) {
    for (unsigned int i = 0; i < length; i++) {
        if (read_buffer.is_empty()) {     // If the "read_buffer" is empty
            __bis_SR_register(LPM0_bits); // Waits until a new character is received (goes to low-power-mode)
            __no_operation();             // Empty line for debugger
        }
        read_buffer.read(&buffer[i]);     // Now we can read the character from the buffer
    }
}

void Uart::skip_until(unsigned char *sample, unsigned int sample_length) {
    unsigned char byte;
    unsigned int matching = 0;

    while (true) {
        Uart::read(&byte, 1);

        if (byte == sample[matching]) {  // If the next part of the sample matches
            matching++;                  // goes forward
        }

        if (matching == sample_length) { // If matching is equal to sample length we're done
            break;
        }
    }
}

#pragma vector=USCI_A0_VECTOR
__interrupt void usci_a0_isr() {
    if (UCA0IFG & UCTXIFG) {
        unsigned char tmp;
        write_buffer.read(&tmp);
        UCA0TXBUF = tmp;
    }

    if (UCA0IFG & UCRXIFG) {                  // A character is ready to be read
        read_buffer.write(UCA0RXBUF);         // Writes the character on the "read_buffer"
        __bic_SR_register_on_exit(LPM0_bits); // Clears low-power-mode (0), and makes the main code run again
    }
}
