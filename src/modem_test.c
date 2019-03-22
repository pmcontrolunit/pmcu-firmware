#include <msp430.h>
#include "uart.h"

/*
void init_12mhz_mclk() {
    UCSCTL3 |= SELREF_2;

    __bis_SR_register(SCG0);
    UCSCTL0 = 0x0000;
    UCSCTL1 = DCORSEL_5;
    UCSCTL2 = FLLD_1 + 374;
    __bic_SR_register(SCG0);

    __delay_cycles(375000);
}

unsigned char received;

int main () {
    WDTCTL = WDTPW | WDTHOLD;

    // init 12mhz clock, needed for 115200 baud-rate communication
    init_12mhz_mclk();

    __bis_SR_register(GIE);

    uart_setup(uart_a0, uart_baud_rate_115200_12mhz);
    uart_setup(uart_a1, uart_baud_rate_115200_12mhz);

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
