#include "circular_buffer.h"

CircularBuffer::CircularBuffer(int size) {
    this->size = size;
    this->write_position = 0;
    this->read_position = 0;
    this->count = 0;
    this->buffer = new unsigned char[size * sizeof(unsigned char)];
}

CircularBuffer::~CircularBuffer() {
    delete[] this->buffer;
}

bool CircularBuffer::is_full() {
    return this->count == this->size;
}

bool CircularBuffer::is_empty() {
    return this->count == 0;
}

bool CircularBuffer::write(unsigned char element) {
    if (!this->is_full()) {
        this->count++;
        this->buffer[this->write_position] = element;
        this->write_position = ++this->write_position % this->size;
        return true;
    }
    return false;
}

bool CircularBuffer::read(unsigned char *element) {
    if (!this->is_empty()) {
        this->count--;
        this->buffer[this->read_position] = element;
        this->read_position = ++this->read_position % this->size;
        return true;
    }
    return false;
}
