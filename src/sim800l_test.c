#include <msp430.h>
#include "modem.h"

#include <string.h>

/*
int main() {
    char result[256];
    const char *data = "HELLO WORLD!";

    WDTCTL = WDTPW | WDTHOLD;

    __bis_SR_register(GIE);

    if (modem_init()) {
        __no_operation();
    }

    if (modem_connect("51.77.151.174", "8080") {
        __no_operation();
    }
    modem_send(NULL, data, strlen(data), 0);

    modem_disconnect();

    __bis_SR_register(LPM0_bits);
    __no_operation();
}
*/
