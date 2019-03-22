#include <msp430.h>

#include "uart.h"
#include "uart_hub.h"

/*
int main() {
    WDTCTL = WDTPW | WDTHOLD;

    __bis_SR_register(GIE);

    uart_hub_init();
    uart_setup(uart_a0, uart_baud_rate_9600);

    uart_hub_select(1);

    while (1) {
        uart_write_str(uart_a0, "\0");
    }
}
*/
