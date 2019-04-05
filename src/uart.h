#ifndef UART_H_
#define UART_H_

#include "circular_buffer.h"
#include "stdint.h"
#include "string.h"

#include <msp430.h>

/* -------------
 * UART MODULES:
 * -------------
 * On MSP430F5529 there are 2 USCI modules: A0 and A1.
 * - A0 on UART mode, ends on 3.3 and 3.4 pins.
 * - A1 on UART mode ends on 4.4 and 4.5 pins, linked with USB serial.
 */
typedef unsigned short uart_module;

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


/* --------------
 * UART SETTINGS:
 * --------------
 * A single byte that contains all configurable parts of UART communication (matching with UCAxCTL0 register).
 * The last 3 bits (lsb) are used for baud-rate configuration.
 */
typedef unsigned char uart_settings;
typedef void (*uart_rx_listener)(char);

#define uart_parity_enabled UCPEN
#define uart_even_parity    UCPAR
#define uart_msb_first      UCMSB
#define uart_7bits_data     UC7BIT
#define uart_stop_bit       UCSPB

#define uart_baud_rate_9600   0b000

#define uart_baud_rate_115200_32khz 0b100
#define uart_baud_rate_115200_1mhz  0b101
#define uart_baud_rate_115200_12mhz 0b110

/*
 * Sets up the given UART module.
 */
void uart_setup(uart_module module, uart_settings settings);

/*
 * Writes the byte out of the given UART module.
 * Returns the number of byte written, 1 if success, 0 if timed out.
 */
int uart_write(uart_module module, uint8_t byte);

/*
 * Writes the bytes buffer out of the given UART module.
 * Returns the number of bytes written, < of buffer_length if timed out.
 */
int uart_write_buffer(uart_module module, uint8_t *buffer, unsigned int buffer_length);

/*
 * Writes the string out of the given UART module.
 * Returns the number of bytes written, < of strlen(string) if timed out.
 */
int uart_write_string(uart_module module, const uint8_t *string);

/*
 * Reads a byte from the given UART module.
 * Returns the number of bytes read, 1 if success, 0 if timed out.
 */
int uart_read(uart_module module, uint8_t *byte);

/*
 * Reads a bytes buffer from the given UART module.
 * Returns the number of bytes read.
 */
int uart_read_buffer(uart_module module, uint8_t *buffer, unsigned int buffer_length);

/*
 * Reads a line from the given UART module.
 * Stops reading when it reaches a \r, a \n or both.
 * Returns the number of bytes read.
 */
int uart_read_line(uart_module module, uint8_t *buffer, unsigned int buffer_length);

/*
 * Reads until it reaches the sample (included) from the UART module.
 * If the buffer isn't enough to store the data, the first values will be replaced.
 * Returns the number of bytes read.
 */
int uart_read_until(uart_module module, const uint8_t *sample, unsigned int sample_length, uint8_t *buffer, unsigned int buffer_length);

#endif
