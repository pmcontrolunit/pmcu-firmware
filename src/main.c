
#include <msp430.h>

#include "uart.h"
#include "gps.h"
/*
int main() {
    unsigned char buffer[1024];

    WDTCTL = WDTPW | WDTHOLD;

    uart_init();

    gps_read_sentence("$GPGGA", buffer, 0, 1024); // Position information
    gps_read_sentence("$GPGSA", buffer, 0, 1024); // Satellite information

    while (1);
}
*/
