#ifndef UART_H_
#define UART_H_

#include "circular_buffer.h"
#include <msp430.h>

#define uart_a0 0x05C0
#define uart_a1 0x0600

#define uart_ctl1  0x00
#define uart_ctl0  0x01
#define uart_br0   0x06
#define uart_br1   0x07
#define uart_mctl  0x08
#define uart_stat  0x0A
#define uart_rxbuf 0x0C
#define uart_txbuf 0x0E
#define uart_abctl 0x10
#define uart_rtctl 0x12
#define uart_rrctl 0x13
#define uart_ie    0x1C
#define uart_ifg   0x1D
#define uart_iv    0x1E

#define uart_register(module, offset) *((unsigned char *) module + offset)

#define uart_parity_enabled UCPEN
#define uart_even_parity    UCPAR
#define uart_msb_first      UCMSB
#define uart_7bits_data     UC7BIT
#define uart_stop_bit       UCSPB

#define uart_baud_rate_9600   0b000
#define uart_baud_rate_115200 0b001

typedef unsigned short uart_module;
typedef unsigned char uart_settings;

void uart_setup(uart_module module, uart_settings settings);

/*
 * Writes the given "buffer", of some "buffer_length", to UART TX through a write_buffer.
 *
 * Returns 0 if all goes well.
 */
int uart_write(uart_module module, unsigned char *buffer, unsigned int buffer_length);

/*
 * Reads from UART RX port until the "buffer", of some "buffer_length", is filled up.
 *
 * Returns 0 if all goes well.
 * Could return 1 in case of time-out.
 */
int uart_read(uart_module module, unsigned char *buffer, unsigned int buffer_length);

/*
 * Reads from UART RX port until the "sample", of some "sample_length" is matched.
 *
 * Writes the bytes on the parameter "buffer", starting from "buffer_offset" index.
 * If the "buffer_length" is exceeded, goes back and replaces old values.
 *
 * Returns 0 if all goes well.
 */
int uart_read_until(uart_module module, unsigned char *sample, unsigned int sample_length, unsigned char *buffer, unsigned int buffer_offset, unsigned int buffer_length);

#endif
