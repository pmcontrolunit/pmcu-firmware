#include <msp430.h>
#include "uart.h"

/*
int main() {
    unsigned char byte;

    unsigned char message[256];
    unsigned int i = 0;

    WDTCTL = WDTPW | WDTHOLD;

    __bis_SR_register(GIE);

    uart_setup(uart_a1, uart_baud_rate_9600);
    while (1) {
        uart_write(uart_a1, "Type something: ", 16);

        while (1) {
            uart_read(uart_a1, &byte, 1);
            message[i++] = byte;
            if (byte == '\r' || byte == '\n') {
                   break;
            }
        }

        uart_write(uart_a1, "You just wrote:\n", 16);
        uart_write(uart_a1, message, i);
        i = 0;
    }
}
*/

