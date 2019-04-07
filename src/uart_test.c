#include <msp430.h>
#include "uart.h"

int main() {
    unsigned char message[256];

    WDTCTL = WDTPW | WDTHOLD;

    __bis_SR_register(GIE);

    uart_setup(UART_A1, UART_BAUD_RATE_9600_SMCLK_1MHZ);

    while (1) {
        uart_write_string(UART_A1, "Type something:\r\n");

        unsigned int size = uart_read_line(UART_A1, message, 256);
        message[size] = '\0';

        uart_write_string(UART_A1, "You wrote: ");
        uart_write_string(UART_A1, message);
    }
}

