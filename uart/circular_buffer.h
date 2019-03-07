#ifndef CIRCULAR_BUFFER_H_
#define CIRCULAR_BUFFER_H_

class CircularBuffer {
private:
    unsigned int size;
    int write_position;
    int read_position;
    unsigned int count;
    unsigned char *buffer;

public:
    CircularBuffer(int size);
    ~CircularBuffer();

    unsigned int get_count();

    unsigned int get_size();

    bool is_full();

    bool is_empty();

    bool write(unsigned char element);

    bool read(unsigned char *element);
};

#endif
