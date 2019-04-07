#include <msp430.h>
#include "uart.h"

int main() {
    unsigned char message[256];

    WDTCTL = WDTPW | WDTHOLD;

    __bis_SR_register(GIE);

    uart_setup(UART_A1, UART_BAUD_RATE_9600_SMCLK_1MHZ);

    while (1) {
        uart_write_string(UART_A1, "Type something:\r\n");
        uart_read_input(UART_A1, message, 256);
        uart_write_string(UART_A1, message);
        uart_write_string(UART_A1, "\r\n");
    }
}

