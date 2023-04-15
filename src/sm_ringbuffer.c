#include <string.h>
#include <stdatomic.h>

#include <sm_ringbuffer.h>

unsigned int pow2roundup(unsigned int x) {
    --x;
    x |= x >> 1;
    x |= x >> 2;
    x |= x >> 4;
    x |= x >> 8;
    x |= x >> 16;
    return x + 1;
}

unsigned int sm_ringbuffer_init(sm_ringbuffer_t *ringbuffer, void *buffer, unsigned int buffer_size,
                                unsigned int element_size) {
    ringbuffer->in = 0;
    ringbuffer->out = 0;
    unsigned int size = pow2roundup(buffer_size / element_size);
    ringbuffer->mask = size - 1;
    ringbuffer->element_size = element_size;
    ringbuffer->data = buffer;
}

unsigned int sm_ringbuffer_push(sm_ringbuffer_t *ringbuffer, void *data) {
    if (sm_ringbuffer_full(ringbuffer))
        return 0;

    void *dest_data = ringbuffer->data + (ringbuffer->in & ringbuffer->mask) * ringbuffer->element_size;
    memcpy(dest_data, data, ringbuffer->element_size);
    atomic_signal_fence(memory_order_release);
    ringbuffer->in++;

    return 1;
}

unsigned int sm_ringbuffer_pop(sm_ringbuffer_t *ringbuffer, void *data) {
    if (sm_ringbuffer_empty(ringbuffer))
        return 0;

    void *src_data = ringbuffer->data + (ringbuffer->out & ringbuffer->mask) * ringbuffer->element_size;
    memcpy(data, src_data, ringbuffer->element_size);
    atomic_signal_fence(memory_order_release);
    ringbuffer->out++;

    return 1;
}

unsigned int sm_ringbuffer_empty(sm_ringbuffer_t *ringbuffer) {
    return ringbuffer->in == ringbuffer->out;
}

unsigned int sm_ringbuffer_full(sm_ringbuffer_t *ringbuffer) {
    return sm_ringbuffer_length(ringbuffer) == ringbuffer->mask + 1;
}

unsigned int sm_ringbuffer_length(sm_ringbuffer_t *ringbuffer) {
    return ringbuffer->in - ringbuffer->out;
}