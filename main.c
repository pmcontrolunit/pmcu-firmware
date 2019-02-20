#include <msp430.h>

enum {
    DHT22_IDLE,

    DHT22_START_SIGNAL,

    DHT22_DATA_PREPARATION,

    DHT22_DATA_STREAM,

} dht22_state = DHT22_IDLE;

unsigned short dht22_last_time;

unsigned long long dht22_processing = 0;

char dht22_int_rh = 0xff;
char dht22_dec_rh = 0xff;

char dht22_int_temp = 0xff;
char dht22_dec_temp = 0xff;


void dht22_read (void) {
    __enable_interrupt();

    dht22_state = DHT22_START_SIGNAL;

    dht22_processing = 1;                                                       // Resets the stream with "communication-end-bit" set

    // Outs a low signal on 1.2
    P1DIR |= BIT2;
    P1SEL &= ~BIT2;
    P1OUT &= ~BIT2;

    TA0CCTL1 = CCIE;    // Enables interrupts
    TA0CCTL1 &= ~CAP;   // Disables capturing mode
    TA0CCTL1 &= ~CCIFG; // Clears interrupt flag

    TA0CTL = TACLR;                  // Clears current timer value
    TA0CTL = TASSEL_2 | MC_2 | ID_0; // Timer settings to run every microsecond
    TA0CTL &= ~TAIE;                 // No interrupt for overflow
    TA0CCR1 = 5000;                  // 5000us = 5ms
}

#pragma vector=TIMER0_A1_VECTOR
void dht22_timer_a (void) {
    switch (dht22_state) {
    case DHT22_START_SIGNAL:
        dht22_state = DHT22_DATA_PREPARATION;

        P1REN |= BIT2;  // Enables input resistor
        P1OUT |= BIT2;  // Pull-up resistor
        P1DIR &= ~BIT2; // 1.2 as input
        P1SEL |= BIT2;  // Special behavior, link with Timer_A0.1

        // Sets timer to capture on rising edge, on CCIxA (linked to 1.2 port)
        TA0CCTL1 = CAP | CM_3 | CCIS_0 | SCS | CCIE;
        break;

    case DHT22_DATA_PREPARATION:
        dht22_state = DHT22_DATA_STREAM;
        break;

    case DHT22_DATA_STREAM:
        if (TA0CCTL1 & CM_2) {                                                  // If was capturing on falling edge
            dht22_last_time = TA0CCR1 - dht22_last_time;                        // Gets time difference

            dht22_processing <<= 1;                                             // Shifts the stream of 1 bit to the left
            if (dht22_last_time >= 70) {                                        // Reads a "1"
                dht22_processing |= 1;
            }

            if (dht22_processing & 0x010000000000) {                            // If end of communication has been reached
                dht22_state = DHT22_IDLE;
                TA0CTL = MC_0;                                                  // Stops timer

                dht22_int_rh = dht22_processing & 0xFF00000000 >> 8 * 4;        // Reads RH (humidity measure)
                dht22_dec_rh = dht22_processing & 0x00FF000000 >> 8 * 3;

                dht22_int_temp = dht22_processing & 0x0000FF0000 >> 8 * 2;      // Reads temperature
                dht22_dec_temp = dht22_processing & 0x000000FF00 >> 8;

                dht22_last_time = dht22_processing & 0x00000000FF;              // Reads checksum ("last_time" variable is used)

                if (dht22_int_rh + dht22_dec_rh +
                        dht22_int_temp + dht22_dec_temp
                                != dht22_last_time) {                           // Computes checksum
                    P1DIR |= BIT0;                                              // Error: misread values, glows the 1.0 red led
                    P1SEL &= ~BIT0;
                    P1OUT |= BIT0;
                } else {
                    dht22_processing = 0;                                       // If all gone well, "processing" variable is 0
                }
            } else {                                                            // If end of communication hasn't been reached
                TA0CCTL1 = CAP | CM_1 | CCIS_0 | SCS | CCIE;
            }
        } else {
            dht22_last_time = TA0CCR1;                                          // Saves the current timestamp
            TA0CCTL1 = CAP | CM_2 | CCIS_0 | SCS | CCIE;
        }
        break;
    }

    TA0CCTL1 &= ~CCIFG;
}

int main (void) {
    // Start dht22 reading
    dht22_read();

    // Wait until the library has finished processing dht22 signal
    while (dht22_processing) {};

    // Here you can read the variables:
    // dht22_int_temp
    // dht22_dec_temp
    // dht22_int_rh
    // dht22_dec_rh

    // End
    while (1);

    return 0;
}
