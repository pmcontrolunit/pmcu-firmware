#include "uart.h"

#include <msp430.h>
#include "circular_buffer.h"

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
    uart_register(module, uart_ctl1) |= UCSWRST;
    uart_register(module, uart_ctl0) = settings & 0xF8;
    switch (settings & 0x07) {
        case uart_baud_rate_9600:
            uart_set_baud_rate_9600(module);
            break;
        case uart_baud_rate_115200_12mhz:
            uart_set_baud_rate_115200_12mhz(module);
            break;
        default:
            break; // error
    }
    switch (module) {
        case uart_a0:
            P3SEL |= BIT3 + BIT4;
            circular_buffer_init(&read_buffer_a0);
            break;
        case uart_a1:
            P4SEL |= BIT4 + BIT5;
            circular_buffer_init(&read_buffer_a1);
            break;
        default:
            break; // error
    }
    uart_register(module, uart_ctl1) &= ~UCSWRST;
    uart_register(module, uart_ie) |= UCRXIE;
}

int uart_write(uart_module module, unsigned char *buffer, unsigned int buffer_offset, unsigned int buffer_length) {
    unsigned int i;
    for (i = 0; i < buffer_length; i++) {
        while (!(uart_register(module, uart_ifg) & UCTXIFG));
        uart_register(module, uart_txbuf) = buffer[i + buffer_offset];
    }
    return 0;
}

int uart_write_str(uart_module module, unsigned char *str) {
    unsigned i = 0;
    while (str[i] != '\0') {
        uart_write(module, &str[i++], 0, 1);
    }
    return 0;
}

int uart_read(uart_module module, unsigned char *buffer, unsigned int buffer_length) {
    circular_buffer *read_buffer = module == uart_a0 ? &read_buffer_a0 : &read_buffer_a1;
    unsigned int i;
    for (i = 0; i < buffer_length; i++) {

        while (circular_buffer_is_empty(read_buffer));
        circular_buffer_read(read_buffer, &buffer[i]);
    }
    return 0;
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
        uart_read(module, &buffer[buffer_i], 1);

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

#pragma vector=USCI_A0_VECTOR
__interrupt void usci_a0_isr() {
    if (UCA0IFG & UCRXIFG) {
        circular_buffer_write(&read_buffer_a0, UCA0RXBUF);
    }
}

#pragma vector=USCI_A1_VECTOR
__interrupt void usci_a1_isr() {
    if (UCA1IFG & UCRXIFG) {
        circular_buffer_write(&read_buffer_a1, UCA1RXBUF);
    }
}
