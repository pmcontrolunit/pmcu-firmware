#ifndef CIRCULAR_BUFFER_H_
#define CIRCULAR_BUFFER_H_

class CircularBuffer {
    int size;
    int write_position;
    int read_position;
    int count;
    unsigned char *buffer;

    CircularBuffer(int size);
    ~CircularBuffer();

    bool is_full();

    bool is_empty();

    bool write(unsigned char element);

    bool read(unsigned char *element);
};

#endif
