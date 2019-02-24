#include "dht22.h"

enum {
    IDLE,
    ACK,
    STREAM,
} state = IDLE;

unsigned int timestamp;
unsigned long long stream;

// Accessible
unsigned int latest_error = DHT22_FLOATING_ERROR;
int rh;
int temperature;

int dht22_get_rh() {
    return rh;
}

int dht22_get_temperature() {
    return temperature;
}

unsigned int dht22_get_latest_error() {
    return latest_error;
}

unsigned int dht22_read() {
    if (state != IDLE) {
        return DHT22_FLOATING_ERROR;
    }

    // Initial setup
    stream = 1;
    state = IDLE;

    __enable_interrupt();

    // Sets up MCLK and SMCLK to run at 8MHz
    UCSCTL3 = SELREF_2;
    UCSCTL0 = 0;
    UCSCTL1 = DCORSEL_5;
    UCSCTL2 |= 249;

    __bic_SR_register(SCG0);

    __delay_cycles(250000);

    // Sends a low signal of 1ms
    P1DIR |= BIT2;
    P1SEL &= ~BIT2;
    P1OUT &= ~BIT2;

    __delay_cycles(8000);

    state = ACK;

    // Sets pin as input, pull-up will make the signal go high
    P1DIR &= ~BIT2;
    P1SEL |= BIT2;
    P1REN |= BIT2;
    P1OUT |= BIT2;

    // Prepares to catpure a signal on the rising edge with a 1us timer
    timestamp = TA0R;
    TA0CTL = TASSEL_2 | MC_2 | ID_3;
    TA0CCTL1 = CAP | CM_1 | CCIS_0 | SCS | CCIE;

    // Loops until the stream is filled or an error is encountred
    while (!(stream & 0x010000000000));

    TA0CCTL1 &= ~CAP;
    TA0CTL = MC_0;

    // If any error has been found stop here
    if (latest_error > DHT22_FLOATING_ERROR) {
        return latest_error;
    }

    {
        unsigned char high_rh;
        unsigned char low_rh;
        unsigned char high_t;
        unsigned char low_t;

        unsigned char checksum;
        unsigned char computed;

        // Extracts values from the read stream
        high_rh = (stream & 0xff00000000) >> (8 * 4);
        low_rh = (stream & 0x00ff000000) >> (8 * 3);
        high_t = (stream & 0x0000ff0000) >> (8 * 2);
        low_t = (stream & 0x000000ff00) >> 8;

        checksum = stream & 0x00000000ff;
        computed = high_rh + low_rh + high_t + low_t;

        // Validates checksum
        if (computed != checksum) {
            latest_error = DHT22_WRONG_CHECKSUM_ERROR;
        } else {
            rh = (high_rh << 8) | low_rh;
            temperature = (high_t << 8) | low_t;
            latest_error = DHT22_NO_ERROR;
        }
    }

    state = IDLE;

    return latest_error;
}

#pragma vector=TIMER0_A1_VECTOR
__interrupt void timera0_cc1_isr (void) {
    TA0CCTL1 &= ~CCIFG;

    switch (state) {
    case ACK:
        state = STREAM;
        TA0CCTL1 = CAP | CM_1 | CCIS_0 | SCS | CCIE;
        break;

    case STREAM:
        if (TA0CCTL1 & CM_2) {
            unsigned int delta;

            delta = TA0CCR1 - timestamp;
            TA0CCTL1 = CAP | CM_1 | CCIS_0 | SCS | CCIE;

            stream <<= 1;
            if (delta >= 70) {
                stream |= 1;
            }
        } else {
            timestamp = TA0CCR1;
            TA0CCTL1 = CAP | CM_2 | CCIS_0 | SCS | CCIE;
        }
        break;
    }
}
