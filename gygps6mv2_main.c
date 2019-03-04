#include <msp430.h>


unsigned char received[1000];
unsigned int i = 0;

int main() {
    WDTCTL = WDTPW + WDTHOLD;                 // Stop WDT

    UCA0CTL1 |= UCSWRST;                      // **Put state machine in reset**
    UCA0CTL1 |= UCSSEL_1;                     // CLK = ACLK
    UCA0BR0 = 0x03;                           // 32kHz/9600=3.41 (see User's Guide)
    UCA0BR1 = 0x00;                           //
    UCA0MCTL = UCBRS_3+UCBRF_0;               // Modulation UCBRSx=3, UCBRFx=0
    UCA0CTL1 &= ~UCSWRST;                     // **Initialize USCI state machine**
    UCA0IE |= UCRXIE;                         // Enable USCI_A0 RX interrupt

    __bis_SR_register(LPM0_bits + GIE);       // Enter LPM0, interrupts enabled
    __no_operation();                         // For debugger

    return 0;
}

#pragma vector=USCI_A1_VECTOR
__interrupt void usci_a1_isr() {
    switch(__even_in_range(UCA0IV,4))
    {
    case 0:break;                             // Vector 0 - no interrupt
    case 2:                                   // Vector 2 - RXIFG
      while (!(UCA0IFG &UCTXIFG));             // USCI_A0 TX buffer ready?
      received[i++] = UCA0RXBUF;
      break;
    case 4:break;                             // Vector 4 - TXIFG
    default: break;
    }
}

