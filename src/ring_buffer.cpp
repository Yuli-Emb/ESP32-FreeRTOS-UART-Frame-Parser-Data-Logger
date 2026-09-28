#include "ring_buffer.h"

uint8_t rb_write(Ring_Buffer *rb, uint8_t byte) {
    if (((rb->head + 1) & (RING_BUFFER_SIZE - 1) )== rb->tail){
        rb->overrun++;
        return 1;
    }

    rb->buf[rb->head] = byte;

    rb->head = (rb->head + 1) & (RING_BUFFER_SIZE - 1);

    return 0;
}

uint8_t rb_read(Ring_Buffer *rb, uint8_t *byte) {
    if (rb->head == rb->tail){
        return 1;
    }

    *byte = rb->buf[rb->tail];

    rb->tail = (rb->tail + 1) & (RING_BUFFER_SIZE - 1);
    
    return 0;
}

uint32_t rb_available(Ring_Buffer *rb) {
    return (rb->head - rb->tail) & (RING_BUFFER_SIZE - 1);
}