#include <msp430.h>
#include "uart.h"

int main () {
    unsigned char buffer[1024];

    WDTCTL = WDTPW | WDTHOLD;

    uart_init_115200();

    uart_write("AT+IPR=9600\n", 12);
    uart_read(buffer, 1024);

    while (1);
}
