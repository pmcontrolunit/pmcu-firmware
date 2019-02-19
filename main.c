#include <msp430.h>

enum {
    DHT22_IDLE,

    DHT22_START_SIGNAL,

    DHT22_DATA_PREPARATION,

    DHT22_DATA_STREAM,

} dht22_state = DHT22_IDLE;

unsigned short dht22_last_time;

unsigned long long dht22_processing = 0;

char dht22_int_rh = -1;
char dht22_dec_rh = -1;

char dht22_int_temp = -1;
char dht22_dec_temp = -1;


void dht22_read (void) {
    dht22_state = DHT22_START_SIGNAL;

    dht22_processing = 1;                                                       // Resets the stream with "communication-end-bit" set

    P1DIR |= BIT2;                                                              // 1.2 as output
    P1SEL &= ~BIT2;                                                             // Disables default behavior
    P1OUT &= ~BIT2;                                                             // Sets 1.2 low

    TA0CTL = TASSEL_2 | MC_2 | ID_0;                                            // Sets the timer to run each 1 microsecond
    TA0CCR1 = 1000;                                                             // Counts 1ms

    TA0CCTL1 &= ~CAP;                                                           // Disables capture mode
    TA0CCTL1 &= ~CCIFG;                                                         // Disables interrupt flag
    TA0CCTL1 |= CCIE;                                                           // Enables interrupts
}

#pragma vector=TIMER0_A0_VECTOR
void dht22_timer_a (void) {
    TA0CCTL1 &= ~CCIFG;

    switch (state) {
    case DHT22_START_SIGNAL:
        dht22_state = DHT22_DATA_PREPARATION;

        P1DIR &= ~BIT2;                                                         // 1.2 as input
        P1REN &= ~BIT2;                                                         // Disables input resistors
        P1SEL |= BIT2;                                                          // Enables special behavior for capturing

        TA0CCTL1 |= CAP;                                                        // Enables capture mode for TA0.1
        TA0CCTL1 |= CCIS_0;                                                     // Selects CCIxA input (attached to 1.2)
        TA0CCTL1 |= SCS;                                                        // Sync captured input with clock (recommended)

        TA0CCTL1 |= CM_1;                                                       // Captures on rising edge
        break;

    case DHT22_DATA_PREPARATION:
        dht22_state = DHT22_DATA_STREAM;

        TA0CCTL1 |= CM_1;                                                       // Captures on rising edge
        break;
    }

    case DHT22_DATA_STREAM:
        if (TA0CCTL1 & CM_2) {                                                  // If was capturing on falling edge
            dht22_last_time = TA0CCR1 - last_time;                              // Gets time difference
            if (dht22_last_time >= 70) {                                        // Reads a "1"
                dht22_processing |= 1;
            }
            dht22_processing <<= 1;                                             // Shifts the stream of 1 bit to the left

            if (processing & 0x010000000000) {                                  // If end of communication has been reached
                dht22_state = DHT22_IDLE;
                TA0CTL = MC_0;                                                  // Stops timer

                dht22_int_rh = dht22_processing & 0xFF00000000 >> 8 * 4;        // Reads RH (humidity measure)
                dht22_dec_rh = dht22_processing & 0x00FF000000 >> 8 * 3;

                dht22_int_temp = dht22_processing & 0x0000FF0000 >> 8 * 2;      // Reads temperature
                dht22_dec_temp = dht22_processing & 0x000000FF00 >> 8;

                dht22_processing = dht22_processing & 0x00000000FF;             // Reads checksum

                if (dht22_int_rh + dht22_dec_rh +
                        dht22_int_temp + dht22_dec_temp
                                != dht22_processing) {                          // Computes checksum
                    P1DIR |= BIT0;                                              // Error: misread values, glows the 1.0 red led
                    P1SEL &= ~BIT0;
                    P1OUT |= BIT0;
                } else {
                    dht22_processing = 0;                                       // If all gone well, "processing" variable is 0
                }
            } else {                                                            // If end of communication hasn't been reached
                TA0CCTL1 |= CM_1;                                               // Waits for another bit
            }
        } else {
            dht22_last_time = TA0CCR1;                                          // Saves the current timestamp
            TA0CCTL1 |= CM_2;                                                   // Captures on falling edge
        }
        break;
}

int main (void) {
    // Start dht22 reading
    dht22_read();

    // Wait until the library has finished processing dht22 signal
    while (dht22_processing);

    // Here you can read the variables:
    // dht22_int_temp
    // dht22_dec_temp
    // dht22_int_rh
    // dht22_dec_rh

    return 0;
}
