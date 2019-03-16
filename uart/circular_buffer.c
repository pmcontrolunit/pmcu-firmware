#include "circular_buffer.h"

void circular_buffer_init(circular_buffer *buffer) {
    buffer->count = 0;
    buffer->write_position = 0;
    buffer->read_position = 0;
}

int circular_buffer_is_empty(circular_buffer *buffer) {
    return buffer->count == 0;
}

int circular_buffer_is_full(circular_buffer *buffer) {
    return buffer->count == CIRCULAR_BUFFER_DATA_SIZE;
}

int circular_buffer_write(circular_buffer *buffer, unsigned char element) {
    if (!circular_buffer_is_full(buffer)) {
        buffer->count++;
        buffer->data[buffer->write_position] = element;
        buffer->write_position = (buffer->write_position + 1) % CIRCULAR_BUFFER_DATA_SIZE;
        return 1;
    }
    return 0;
}

int circular_buffer_read(circular_buffer *buffer, unsigned char *element) {
    if (!circular_buffer_is_empty(buffer)) {
        buffer->count--;
        *element = buffer->data[buffer->read_position];
        buffer->read_position = (buffer->read_position + 1) % CIRCULAR_BUFFER_DATA_SIZE;
        return 1;
    }
    return 0;
}
