#include <msp430.h>
#include "time.h"


int main(void) {
    WDTCTL = WDTPW | WDTHOLD;

    start_timing();
    while(1);
}
