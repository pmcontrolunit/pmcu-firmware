#ifndef UART_H_
#define UART_H_

#include <msp430.h>
#include "stdint.h"

/*
 * A type representing the UART module.
 * On MSP430F5529 are available 2 USCI modules: A0 and A1.
 * - A0 ends on 3.3 (TX) and 3.4 (RX) pins.
 * - A1 ends on 4.4 (TX) and 4.5 (RX) pins.
 */
typedef uint16_t uart_module;

#define UART_A0 0x05C0
#define UART_A1 0x0600

#define UART_CTL1  0x00
#define UART_CTL0  0x01
#define UART_BR0   0x06
#define UART_BR1   0x07
#define UART_MCTL  0x08
#define UART_STAT  0x0A
#define UART_RXBUF 0x0C
#define UART_TXBUF 0x0E
#define UART_ABCTL 0x10
#define UART_RTCTL 0x12
#define UART_RRCTL 0x13
#define UART_IE    0x1C
#define UART_IFG   0x1D
#define UART_IV    0x1E

#define UART_REGISTER(module, offset) *((uint16_t *) module + offset)

/*
 * A type representing the UART settings.
 * A single byte that contains all configurable parts of UART communication (matching with UCAxCTL0 register).
 * The last 3 bits (lsb) are used for baud-rate configuration.
 */
typedef uint8_t uart_settings;

#define UART_PARITY_ENABLED UCPEN
#define UART_EVEN_PARITY    UCPAR
#define UART_MSB_FIRST      UCMSB
#define UART_7BITS_DATA     UC7BIT
#define UART_STOP_BIT       UCSPB

#define UART_BAUD_RATE_9600_SMCLK_1MHZ 0b000
#define UART_BAUD_RATE_9600_ACLK_32KHZ 0b001

#define UART_BAUD_RATE_115200_SMCLK_12MHZ 0b100
#define UART_BAUD_RATE_115200_SMCLK_1MHZ  0b101

/*
 * After how many seconds the function goes timeout for write operations.
 */
#define UART_WRITE_TIMEOUT 10

/*
 * After how many seconds the function goes timeout for read operations.
 */
#define UART_READ_TIMEOUT 10

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
int uart_read(uart_module module, uint8_t *byte, unsigned short timeout);

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
 * Reads until it reaches the sample (included) from the given UART module.
 * If the buffer isn't enough to store the data, the first values will be replaced.
 * Returns the number of bytes read.
 */
int uart_read_until(uart_module module, const uint8_t *sample, unsigned int sample_length, uint8_t *buffer, unsigned int buffer_length);

/*
 * Reads until it reaches the sample_string (included) from the given UART module.
 * If the buffer isn't enough to store the data, the first values will be replaced.
 * Returns the number of bytes read.
 */
int uart_read_until_string(uart_module module, const char *sample_string, uint8_t *buffer, unsigned int buffer_length);

#endif
