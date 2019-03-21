#include <msp430.h>
#include "uart.h"

void init_12mhz_mclk() {
    UCSCTL3 |= SELREF_2;

    __bis_SR_register(SCG0);
    UCSCTL0 = 0x0000;
    UCSCTL1 = DCORSEL_5;
    UCSCTL2 = FLLD_1 + 374;
    __bic_SR_register(SCG0);

    __delay_cycles(375000);
}

int main () {
    unsigned char buffer[128];
    unsigned int i, j;

    WDTCTL = WDTPW | WDTHOLD;

    // init 12mhz clock, needed for 115200 baud-rate communication
    init_12mhz_mclk();

    __bis_SR_register(GIE);

    uart_setup(uart_a0, uart_baud_rate_115200_12mhz);
    uart_setup(uart_a1, uart_baud_rate_115200_12mhz);

    while (1) {
        uart_write_str(uart_a1, "Insert AT command (type and press enter):\r\n");

        // Reads command and sends it to DCE, silently.
        i = 0;
        while (1) {
            uart_read(uart_a1, &buffer[i], 1);
            if (buffer[i] == '\r') {
                break;
            }
            i++;
        }
        uart_write(uart_a0, buffer, 0, i + 1);

        /*
        // Since DCE echoes input command, writes it back.
        i = 0;
        while (1) {
            uart_read(uart_a0, &buffer[i], 1);
            uart_write(uart_a1, &buffer[i], 0, 1);

            if (buffer[i] == '\r') {
                break;
            }
            i++;
        }
        uart_write_str(uart_a1, "\n");

        // Prints DCE response, format is: <CR><LF>response<CR><LF>
        i = 0;
        while (1) {
            uart_read(uart_a0, &buffer[i], 1);
            if (buffer[i] == '\r' || i > 0) {
                uart_write(uart_a0, &buffer[i], 0, 1);
                i++;
            }
            if (buffer[i] == '\n' && i > 1) {
                break;
            }
        }
        */

        j = 0;
        while (j != 2) {
            uart_read(uart_a0, &buffer[i], 1);
            uart_write(uart_a1, &buffer[i], 0, 1);
            if (buffer[i] == '\n') {
                j++;
            }
            i++;
        }
    }
}
