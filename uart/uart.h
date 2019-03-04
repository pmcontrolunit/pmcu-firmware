#ifndef UART_H_
#define UART_H_

#include "circular_buffer.h"

namespace Uart {
    typedef void (*OnReadListener)(char);

    OnReadListener *read_listener;
    CircularBuffer *write_buffer;

    void init();

    void select(int select, OnReadListener& read_listener);

    void write(char *buffer, int length);
}

#endif
