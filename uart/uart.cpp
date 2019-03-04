#include "uart.h"
#include <msp430.h>

using namespace Uart;

char write_buffer[16];

void Uart::init() {
    // TODO setup UART

}

void Uart::select(int select, OnReadListener& read_listener) {
    Uart::read_listener = &read_listener;
}

void Uart::write(char *buffer, int length) {
    Uart::write_buffer[buffer];
}

#pragma vector=USCI_A1_VECTOR
__interrupt void usci_a1_isr() {
    if (UCA0IFG & UCTXIFG) {
        UCA0TXBUF = Uart::write_buffer[]
    }

    if (UCA0IFG & UCRXIFG) {
        Uart::read_listener();
    }
}
