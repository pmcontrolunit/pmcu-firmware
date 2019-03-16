#include "gps.h"

#include "uart/uart.h"
#include <msp430.h>

int gps_read_sentence(unsigned char *sentence_type, unsigned char *buffer, unsigned int buffer_length) {
    // Waits for the given "sentence_type", skips the incoming bytes
    uart_read_until(sentence_type, 6, 0, 0);

    // Waits for the next $, stores the middle bytes on "buffer"
    uart_read_until("$", 0, buffer, buffer_length);
    return 0;
}

int gps_read() {
    unsigned char sentence[1024];

    gps_read_sentence("$GPGGA", sentence, 1024);
    __no_operation();

    gps_read_sentence("$GPGSA", sentence, 1024);
    __no_operation();
    return 0;
}

/*
int main() {
    WDTCTL = WDTPW | WDTHOLD;

    uart_init();

    gps_read();

    while (1);
}
*/
