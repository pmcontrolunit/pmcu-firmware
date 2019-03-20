#include "gps.h"

#include "uart.h"
#include <msp430.h>

int gps_read_sentence(unsigned char *sentence_type, unsigned char *buffer, unsigned int buffer_offset, unsigned int buffer_length) {
    uart_read_until(uart_a0, sentence_type, 6, 0, 0, 0);
    uart_read_until(uart_a0, "$", 0, buffer, buffer_offset, buffer_length);
    return 0;
}
