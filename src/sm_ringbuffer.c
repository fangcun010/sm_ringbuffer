#include <string.h>
#include <stdatomic.h>

#include <sm_ringbuffer.h>

unsigned int pow2rounddown(unsigned int x) {
    unsigned int count = 0;

    while (x) {
        x >>= 1;
        count++;
    }

    return 1 << (count - 1);
}

unsigned int sm_ringbuffer_init(sm_ringbuffer_t *ringbuffer, void *buffer, unsigned int buffer_size,
                                unsigned int element_size) {
    ringbuffer->in = 0;
    ringbuffer->out = 0;
    unsigned int size = pow2rounddown(buffer_size / element_size);
    ringbuffer->mask = size - 1;
    ringbuffer->element_size = element_size;
    ringbuffer->data = buffer;

    return 1;
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

unsigned int sm_ringbuffer_peek_in_place(sm_ringbuffer_t *ringbuffer, void **data) {
    if (sm_ringbuffer_empty(ringbuffer))
        return 0;

    void *src_data = ringbuffer->data + (ringbuffer->out & ringbuffer->mask) * ringbuffer->element_size;
    *data = src_data;

    return 1;
}

unsigned int sm_ringbuffer_skip_one(sm_ringbuffer_t *ringbuffer) {
    atomic_signal_fence(memory_order_release);
    ringbuffer->out++;

    return 1;
}

unsigned int sm_ringbuffer_poke_in_place(sm_ringbuffer_t *ringbuffer, void **data) {
    if (sm_ringbuffer_full(ringbuffer))
        return 0;

    void *dest_data = ringbuffer->data + (ringbuffer->in & ringbuffer->mask) * ringbuffer->element_size;
    *data = dest_data;

    return 1;
}

unsigned int sm_ringbuffer_push_in_place(sm_ringbuffer_t *ringbuffer) {
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

unsigned int sm_messagequeue_init(sm_messagequeue_t *messagequeue, void *buffer, unsigned int buffer_size) {
    messagequeue->in = 0;
    messagequeue->out = 0;
    unsigned int size = pow2rounddown(buffer_size);
    messagequeue->mask = size - 1;
    messagequeue->data = buffer;

    return 1;
}

unsigned int sm_messagequeue_push(sm_messagequeue_t *messagequeue, void *data, unsigned int element_size) {
    if (sm_messagequeue_full(messagequeue))
        return 0;

    void *dest_data = messagequeue->data + (messagequeue->in & messagequeue->mask);
    *((unsigned int *) dest_data) = element_size;
    memcpy(dest_data + sizeof(element_size), data, element_size);
    atomic_signal_fence(memory_order_release);
    messagequeue->in += element_size + sizeof(element_size);

    return 1;
}

unsigned int sm_messagequeue_peek_in_place(sm_messagequeue_t *messagequeue, void **data,
                                           unsigned int *element_size) {
    if (sm_messagequeue_empty(messagequeue))
        return 0;

    void *src_data = messagequeue->data + (messagequeue->out & messagequeue->mask);
    *element_size = *((unsigned int *) src_data);
    *data = src_data + sizeof(*element_size);

    return 1;
}

unsigned int sm_messagequeue_skip_one(sm_messagequeue_t *messagequeue) {
    if (sm_messagequeue_empty(messagequeue))
        return 0;

    void *src_data = messagequeue->data + (messagequeue->out & messagequeue->mask);
    unsigned int element_size = *((unsigned int *) src_data);

    atomic_signal_fence(memory_order_release);
    messagequeue->out += element_size + sizeof(element_size);
}

unsigned int sm_messagequeue_pop(sm_messagequeue_t *messagequeue, void *data, unsigned int *element_size) {
    if (sm_messagequeue_empty(messagequeue))
        return 0;

    void *src_data = messagequeue->data + (messagequeue->out & messagequeue->mask);
    *element_size = *(unsigned int *) src_data;
    memcpy(data, src_data + sizeof(*element_size), *element_size);
    atomic_signal_fence(memory_order_release);
    messagequeue->out += *element_size + sizeof(*element_size);

    return 1;
}

unsigned int sm_messagequeue_poke_in_place(sm_messagequeue_t *messagequeue, void **data, unsigned int element_size) {
    if (sm_messagequeue_full(messagequeue))
        return 0;

    void *dest_data = messagequeue->data + (messagequeue->in & messagequeue->mask);
    *((unsigned int *) dest_data) = element_size;
    *data = dest_data + sizeof(element_size);

    return 1;
}

unsigned int sm_messagequeue_push_in_place(sm_messagequeue_t *messagequeue) {
    if (sm_messagequeue_full(messagequeue))
        return 0;

    void *dest_data = messagequeue->data + (messagequeue->in & messagequeue->mask);
    unsigned int element_size = *((unsigned int *) dest_data);
    atomic_signal_fence(memory_order_release);
    messagequeue->in += element_size + sizeof(element_size);

    return 1;
}

unsigned int sm_messagequeue_empty(sm_messagequeue_t *messagequeue) {
    return messagequeue->in == messagequeue->out;
}

unsigned int sm_messagequeue_full(sm_messagequeue_t *messagequeue) {
    return sm_messagequeue_length(messagequeue) == messagequeue->mask + 1;
}

unsigned int sm_messagequeue_length(sm_messagequeue_t *messagequeue) {
    return messagequeue->in - messagequeue->out;
}
