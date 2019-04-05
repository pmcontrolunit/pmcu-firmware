#include "uart.h"

#include <msp430.h>
#include "circular_buffer.h"

#define UART_WRITE_TIMEOUT 10
#define UART_READ_TIMEOUT 10

circular_buffer read_buffer_a0;
circular_buffer read_buffer_a1;

void uart_set_baud_rate_9600(uart_module module) {
    uart_register(module, uart_ctl1) |= UCSSEL_1;
    uart_register(module, uart_br0) = 3;
    uart_register(module, uart_br1) = 0;
    uart_register(module, uart_mctl) = UCBRS_3 + UCBRF_0;
}

void uart_set_baud_rate_115200_12mhz(uart_module module) {
    uart_register(module, uart_ctl1) |= UCSSEL_2;
    uart_register(module, uart_br0) = 104;
    uart_register(module, uart_br1) = 0;
    uart_register(module, uart_mctl) = UCBRS_1 + UCBRF_0;
}

void uart_setup(uart_module module, uart_settings settings) {
    /* Sets up UART specifics */
    uart_register(module, uart_ctl1) |= UCSWRST;
    uart_register(module, uart_ctl0) = settings & 0xF8;

    switch (settings & 0x07) {
        case uart_baud_rate_115200_12mhz:
            uart_set_baud_rate_115200_12mhz(module);
            break;
        default:
        case uart_baud_rate_9600:
            uart_set_baud_rate_9600(module);
            break;
    }
    switch (module) {
        case uart_a0:
            P3SEL |= BIT3 + BIT4;
            break;
        case uart_a1:
            P4SEL |= BIT4 + BIT5;
            break;
        default:
            break; // error
    }
    uart_register(module, uart_ctl1) &= ~UCSWRST;
    uart_register(module, uart_ie) |= UCRXIE;
}

/** TX **/

inline int uart_write(uart_module module, unsigned char byte) {
    return uart_write_buffer(module, &byte, 1);
}

int uart_write_buffer(uart_module module, unsigned char *buffer, unsigned int buffer_length) {
    DMACTL0 = module == uart_a0 ? DMA0TSEL_17 : DMA0TSEL_21; // Sets up trigger for DMA0 on UCATXIFG ready.

    // Setup DMA0
    __data16_write_addr((unsigned short) &DMA0SA,(unsigned long) buffer);                             // From the buffer
    __data16_write_addr((unsigned short) &DMA0DA,(unsigned long) &uart_register(module, uart_txbuf)); // to the UCATXBUF register.

    DMA0SZ = buffer_length;                                     // How much data to transfer, in this case buffer_length.
    DMA0CTL = DMASRCINCR_3 + DMADSTINCR_0 + DMASBDB + DMALEVEL + DMADT_0; // Source address incremented, destination not, byte per byte.

    TA0CTL = TASSEL_1 + MC_1 + ID_3; // Sets up a new timer to
    TA0CCR0 = 4000 * UART_READ_TIMEOUT;     // compute the timeout after
    TA0CCTL0 |= CCIE;                // the UART_WRITE_TIMEOUT interval.

    __bis_SR_register(LPM0_bits); // Halts the CPU, awaked on complete.
    __no_operation();             // For debug purposes.

    TA0CTL = MC0; // Clears the timer.

    // Returns how many bytes has been sent.
    // If != to buffer_length, means it went timeout.
    return buffer_length - DMA0SZ;
}

/** RX **/

int uart_read(uart_module module, unsigned char *byte) {
    circular_buffer read_buffer = module == uart_a0 ? read_buffer_a0 : read_buffer_a1;
    if (circular_buffer_is_empty(&read_buffer)) {
        TA0CTL = TASSEL_1 + MC_1 + ID_3; // Sets up a new timer to
        TA0CCR0 = 4000 * UART_READ_TIMEOUT;     // compute the timeout after
        TA0CCTL0 |= CCIE;                // the UART_WRITE_TIMEOUT interval.

        __bis_SR_register(LPM0_bits); // Halts the CPU, waiting for a
        __no_operation();             // new byte.
    }

    TA0CTL = MC0; // Clears the timer.

    if (circular_buffer_is_empty(&read_buffer)) { // If the buffer is still empty
        return 0;                                 // means it went timeout.
    }
    circular_buffer_read(&read_buffer, byte); // Reads the new byte.
    return 1;
}

int uart_read_buffer(uart_module module, unsigned char *buffer, unsigned int buffer_length) {
    unsigned int i;
    for (i = 0; i < buffer_length; i++) {     // Foreach byte of the buffer
        if (!uart_read(module, &buffer[i])) { // tries to read a new byte.
            break;                            // If at least on goes timeout, exits.
        }
    }
    return i;
}

int uart_read_until(uart_module module, unsigned char *sample, unsigned int sample_length, unsigned char *buffer, unsigned int buffer_offset, unsigned int buffer_length) {
    unsigned int sample_i = 0;
    unsigned int buffer_i = buffer_offset;
    unsigned char fallback = 0;

    if (!buffer) { // If buffer isn't defined (0) points to 1 byte
        buffer = &fallback;
        buffer_offset = 0;
        buffer_length = 1;
    }

    while (1) {
        uart_read_buffer(module, &buffer[buffer_i], 1);

        if (buffer[buffer_i] == sample[sample_i]) {
            sample_i++;
        } else if (sample_i > 0) {
            sample_i = 0;
        }

        if (sample_i == sample_length) {
            break;
        }

        buffer_i = ++buffer_i >= buffer_length ? buffer_i : buffer_offset;
    }
    return 0;
}

#pragma vector=DMA_VECTOR
__interrupt void on_uart_tx_complete() {
    __bic_SR_register_on_exit(LPM0_bits);
    __no_operation();
}

#pragma vector=USCI_A0_VECTOR
__interrupt void on_uart_a0_rx() {
    if (UCA0IFG & UCRXIFG) {
        // If buffer is empty, a read could be waiting, so we remove LPM0
        if (circular_buffer_is_empty(&read_buffer_a0)) {
            __bic_SR_register_on_exit(LPM0_bits);
        }
        circular_buffer_write(&read_buffer_a0, UCA0RXBUF);
    }
}

#pragma vector=USCI_A1_VECTOR
__interrupt void on_uart_a1_rx() {
    if (UCA1IFG & UCRXIFG) {
        circular_buffer_write(&read_buffer_a1, UCA1RXBUF);
    }
}

#pragma vector=TIMER0_A0_VECTOR
__interrupt void on_rx_timeout() {
    __bic_SR_register_on_exit(LPM0_bits);
}

