#ifndef UART_H_
#define UART_H_

#define WRITE_BUFFER_CAPACITY 20

void uart_init();

/*
 * Writes the given "buffer", of some "buffer_length", to UART TX through a write_buffer.
 *
 * Returns 0 if all goes well.
 */
int uart_write(unsigned char *buffer, unsigned int buffer_length);

/*
 * Reads from UART RX port until the "buffer", of some "buffer_length", is filled up.
 *
 * Returns 0 if all goes well.
 * Could return 1 in case of time-out.
 */
int uart_read(unsigned char *buffer, unsigned int buffer_length);

/*
 * Reads from UART RX port until the "sample", of some "sample_length" is matched.
 *
 * Writes the bytes on the parameter "buffer", starting from "buffer_offset" index.
 * If the "buffer_length" is exceeded, goes back and replaces old values.
 *
 * Returns 0 if all goes well.
 */
int uart_read_until(unsigned char *sample, unsigned int sample_length, unsigned char *buffer, unsigned int buffer_offset, unsigned int buffer_length);

#endif
