#include <msp430.h>

#include "uart.h"
#include "uart_hub.h"

/*
int main () {
    WDTCTL = WDTPW | WDTHOLD;

    __bis_SR_register(GIE);

    uart_hub_init();
    uart_hub_select(1);

    uart_setup(UART_A0, UART_BAUD_RATE_9600_SMCLK_1MHZ);
    uart_setup(UART_A1, UART_BAUD_RATE_9600_SMCLK_1MHZ);

    __bis_SR_register(LPM0_bits);

    __no_operation();
}

#pragma vector=USCI_A0_VECTOR
__interrupt void usci_a0_isr() {
    if (UCA0IFG & UCRXIFG) {
        while (!(UCA1IFG & UCTXIFG));
        UCA1TXBUF = UCA0RXBUF;
    }
}

#pragma vector=USCI_A1_VECTOR
__interrupt void usci_a1_isr() {
    if (UCA1IFG & UCRXIFG) {
        while (!(UCA0IFG & UCTXIFG));
        UCA0TXBUF = UCA1RXBUF;
    }
}
*/
