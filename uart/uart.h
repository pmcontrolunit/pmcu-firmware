#ifndef UART_H_
#define UART_H_

#define WRITE_BUFFER_CAPACITY 20

#include "circular_buffer.h"

namespace Uart {
    /*
     * Inits the UART pins for communication.
     * By default, uses pins 3.3, for TX, and 3.4, for RX and sets up a baud-rate of 9600.
     */
    void init();

    /*
     * Writes the given buffer out of the UART pins, through a write buffer.
     * If the write buffer was unable to completely contain the given one, returns false, otherwise true.
     *
     * The write buffer capacity is defined by WRITE_BUFFER_CAPACITY, the given buffer can't
     * exceed that value or this function will always fail.
     */
    bool write(unsigned char *buffer, unsigned int length);

    /*
     * Fills a bytes buffer, of the given length, of the data received from UART RX.
     * The function will block until the buffer is fully satisfied.
     */
    void read(unsigned char *buffer, unsigned int length);

    /*
     * Skips the received bytes (from UART RX), until the given sequence is satisfied and skipped too.
     *
     * If you have a stream like:
     * $PGRMM,WGS 84*06$GPBOD,,T,,M,,*47
     *
     * And you decide to skip_until("$GPBOD"), the final stream will be:
     * ,,T,,M,,*47
     */
    void skip_until(unsigned char *sample, unsigned int length);
}

#endif
