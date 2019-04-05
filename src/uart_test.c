#include <msp430.h>
#include "uart.h"

int main() {
    unsigned char byte;

    unsigned char message[256];
    unsigned int i = 0;

    WDTCTL = WDTPW | WDTHOLD;

    __bis_SR_register(GIE);

    uart_setup(uart_a1, uart_baud_rate_9600);
    while (1) {
        if (!uart_write_buffer(uart_a1, "Type something: ", 16) < 16) {
            __no_operation();
        }

        while (1) {
            if (uart_read(uart_a1, &byte)) {
                message[i++] = byte;
                if (byte == '\r' || byte == '\n') {
                       break;
                }
            } else {
                __no_operation();
            }
        }
        uart_write_buffer(uart_a1, "You just wrote:\n", 16);
        uart_write_buffer(uart_a1, message, i);
        i = 0;
    }
}

