#include <msp430.h>
#include "modem.h"

#include <string.h>

int main() {
    volatile int error;
    WDTCTL = WDTPW | WDTHOLD;

    __bis_SR_register(GIE);

    error = modem_init();

    if (!modem_connect("51.77.151.174", "8080")) {
        const char *data = "HELLO WORLD!";
        modem_sendall(NULL, data, strlen(data), 0);
    }

    __bis_SR_register(LPM0_bits);
    __no_operation();
}
