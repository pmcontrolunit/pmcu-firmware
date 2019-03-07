#include "uart/uart.h"
#include "gps.h"

#include <msp430.h>


unsigned char gga_info[1024];

void Gps::read() {
    unsigned char keyword[6] = {'$', 'G', 'P', 'G', 'G', 'A' };
    Uart::skip_until(keyword, 6);

    Uart::read(gga_info, 1024);
}

int main() {
    WDTCTL = WDTPW | WDTHOLD;

    Uart::init();
    Gps::read();
}
