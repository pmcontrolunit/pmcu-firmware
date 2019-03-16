#include "dht22.h"

#ifdef DHT22_TEST
int main (void) {
    unsigned int error;
    int rh, temperature;

    WDTCTL = WDTPW | WDTHOLD;

    error = dht22_read();
    if (error) {
        return error;
    }

    rh = dht22_get_rh();
    temperature = dht22_get_temperature();

    return 0;
}
#endif
