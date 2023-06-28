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

unsigned int sm_ringbuffer_init(sm_ringbuffer_t *ringbuffer, sm_shared_ringbuffer_data_t *shared_ringbuffer_data,
                                void *buffer, unsigned int buffer_size,
                                unsigned int element_size) {
    ringbuffer->shared_ringbuffer_data = shared_ringbuffer_data;
    shared_ringbuffer_data->in = 0;
    shared_ringbuffer_data->out = 0;
    unsigned int size = pow2rounddown(buffer_size / element_size);
    shared_ringbuffer_data->mask = size - 1;
    shared_ringbuffer_data->element_size = element_size;
    ringbuffer->base_address = buffer;

    return 1;
}

unsigned int sm_ringbuffer_open(sm_ringbuffer_t *ringbuffer, sm_shared_ringbuffer_data_t *shared_ringbuffer_data,
                                void *base_address) {
    ringbuffer->shared_ringbuffer_data = shared_ringbuffer_data;
    ringbuffer->base_address = base_address;

    return 1;
}

unsigned int sm_ringbuffer_push(sm_ringbuffer_t *ringbuffer, void *data) {
    if (sm_ringbuffer_full(ringbuffer))
        return 0;

    sm_shared_ringbuffer_data_t *shared_ringbuffer_data = ringbuffer->shared_ringbuffer_data;
    void *dest_data = ringbuffer->base_address +
                      (shared_ringbuffer_data->in & shared_ringbuffer_data->mask) *
                      shared_ringbuffer_data->element_size;
    memcpy(dest_data, data, shared_ringbuffer_data->element_size);
    atomic_signal_fence(memory_order_release);
    shared_ringbuffer_data->in++;

    return 1;
}

unsigned int sm_ringbuffer_peek_in_place(sm_ringbuffer_t *ringbuffer, void **data) {
    if (sm_ringbuffer_empty(ringbuffer))
        return 0;

    sm_shared_ringbuffer_data_t *shared_ringbuffer_data = ringbuffer->shared_ringbuffer_data;
    void *src_data = ringbuffer->base_address +
                     (shared_ringbuffer_data->out & shared_ringbuffer_data->mask) *
                     shared_ringbuffer_data->element_size;
    *data = src_data;

    return 1;
}

unsigned int sm_ringbuffer_skip_one(sm_ringbuffer_t *ringbuffer) {
    atomic_signal_fence(memory_order_release);
    ringbuffer->shared_ringbuffer_data->out++;

    return 1;
}

unsigned int sm_ringbuffer_poke_in_place(sm_ringbuffer_t *ringbuffer, void **data) {
    if (sm_ringbuffer_full(ringbuffer))
        return 0;

    sm_shared_ringbuffer_data_t *shared_ringbuffer_data = ringbuffer->shared_ringbuffer_data;
    void *dest_data = ringbuffer->base_address + (shared_ringbuffer_data->in & shared_ringbuffer_data->mask) *
                                                 shared_ringbuffer_data->element_size;
    *data = dest_data;

    return 1;
}

unsigned int sm_ringbuffer_push_in_place(sm_ringbuffer_t *ringbuffer) {
    atomic_signal_fence(memory_order_release);
    ringbuffer->shared_ringbuffer_data->in++;

    return 1;
}

unsigned int sm_ringbuffer_pop(sm_ringbuffer_t *ringbuffer, void *data) {
    if (sm_ringbuffer_empty(ringbuffer))
        return 0;

    sm_shared_ringbuffer_data_t *shared_ringbuffer_data = ringbuffer->shared_ringbuffer_data;
    void *src_data = ringbuffer->base_address + (shared_ringbuffer_data->out & shared_ringbuffer_data->mask) *
                                                shared_ringbuffer_data->element_size;
    memcpy(data, src_data, shared_ringbuffer_data->element_size);
    atomic_signal_fence(memory_order_release);
    ringbuffer->shared_ringbuffer_data->out++;

    return 1;
}

unsigned int sm_ringbuffer_empty(sm_ringbuffer_t *ringbuffer) {
    return ringbuffer->shared_ringbuffer_data->in == ringbuffer->shared_ringbuffer_data->out;
}

unsigned int sm_ringbuffer_full(sm_ringbuffer_t *ringbuffer) {
    return sm_ringbuffer_length(ringbuffer) == ringbuffer->shared_ringbuffer_data->mask + 1;
}

unsigned int sm_ringbuffer_length(sm_ringbuffer_t *ringbuffer) {
    return ringbuffer->shared_ringbuffer_data->in - ringbuffer->shared_ringbuffer_data->out;
}

unsigned int sm_ringbuffer_capacity(sm_ringbuffer_t *ringbuffer) {
    return ringbuffer->shared_ringbuffer_data->mask + 1;
}

unsigned int sm_messagequeue_init(sm_messagequeue_t *messagequeue,
                                  sm_shared_messagequeue_data_t *shared_messagequeue_data,
                                  void *buffer, unsigned int buffer_size) {
    messagequeue->shared_messagequeue_data = shared_messagequeue_data;
    messagequeue->base_address = buffer;
    shared_messagequeue_data->in = 0;
    shared_messagequeue_data->out = 0;
    unsigned int size = pow2rounddown(buffer_size);
    shared_messagequeue_data->mask = size - 1;

    return 1;
}

unsigned int sm_messagequeue_open(sm_messagequeue_t *messagequeue,
                                  sm_shared_messagequeue_data_t *shared_messagequeue_data,
                                  void *base_address) {
    messagequeue->shared_messagequeue_data = shared_messagequeue_data;
    messagequeue->base_address = base_address;

    return 1;
}

unsigned int sm_messagequeue_push(sm_messagequeue_t *messagequeue, void *data, unsigned int element_size) {
    if (sm_messagequeue_full(messagequeue))
        return 0;

    sm_shared_messagequeue_data_t *shared_messagequeue_data = messagequeue->shared_messagequeue_data;
    void *dest_data = messagequeue->base_address + (shared_messagequeue_data->in & shared_messagequeue_data->mask);
    *((unsigned int *) dest_data) = element_size;
    memcpy(dest_data + sizeof(element_size), data, element_size);
    atomic_signal_fence(memory_order_release);
    shared_messagequeue_data->in += element_size + sizeof(element_size);

    return 1;
}

unsigned int sm_messagequeue_peek_in_place(sm_messagequeue_t *messagequeue, void **data,
                                           unsigned int *element_size) {
    if (sm_messagequeue_empty(messagequeue))
        return 0;

    sm_shared_messagequeue_data_t *shared_messagequeue_data = messagequeue->shared_messagequeue_data;
    void *src_data = messagequeue->base_address + (shared_messagequeue_data->out & shared_messagequeue_data->mask);
    *element_size = *((unsigned int *) src_data);
    *data = src_data + sizeof(*element_size);

    return 1;
}

unsigned int sm_messagequeue_skip_one(sm_messagequeue_t *messagequeue) {
    if (sm_messagequeue_empty(messagequeue))
        return 0;

    sm_shared_messagequeue_data_t *shared_messagequeue_data = messagequeue->shared_messagequeue_data;
    void *src_data = messagequeue->base_address + (shared_messagequeue_data->out & shared_messagequeue_data->mask);
    unsigned int element_size = *((unsigned int *) src_data);

    atomic_signal_fence(memory_order_release);
    shared_messagequeue_data->out += element_size + sizeof(element_size);
}

unsigned int sm_messagequeue_pop(sm_messagequeue_t *messagequeue, void *data, unsigned int *element_size) {
    if (sm_messagequeue_empty(messagequeue))
        return 0;

    sm_shared_messagequeue_data_t *shared_messagequeue_data = messagequeue->shared_messagequeue_data;
    void *src_data = messagequeue->base_address + (shared_messagequeue_data->out & shared_messagequeue_data->mask);
    *element_size = *(unsigned int *) src_data;
    memcpy(data, src_data + sizeof(*element_size), *element_size);
    atomic_signal_fence(memory_order_release);
    shared_messagequeue_data->out += *element_size + sizeof(*element_size);

    return 1;
}

unsigned int sm_messagequeue_poke_in_place(sm_messagequeue_t *messagequeue, void **data, unsigned int element_size) {
    if (sm_messagequeue_full(messagequeue))
        return 0;

    sm_shared_messagequeue_data_t *shared_messagequeue_data = messagequeue->shared_messagequeue_data;
    void *dest_data = messagequeue->base_address + (shared_messagequeue_data->in & shared_messagequeue_data->mask);
    *((unsigned int *) dest_data) = element_size;
    *data = dest_data + sizeof(element_size);

    return 1;
}

unsigned int sm_messagequeue_push_in_place(sm_messagequeue_t *messagequeue) {
    if (sm_messagequeue_full(messagequeue))
        return 0;

    sm_shared_messagequeue_data_t *shared_messagequeue_data = messagequeue->shared_messagequeue_data;
    void *dest_data = messagequeue->base_address + (shared_messagequeue_data->in & shared_messagequeue_data->mask);
    unsigned int element_size = *((unsigned int *) dest_data);
    atomic_signal_fence(memory_order_release);
    shared_messagequeue_data->in += element_size + sizeof(element_size);

    return 1;
}

unsigned int sm_messagequeue_empty(sm_messagequeue_t *messagequeue) {
    return messagequeue->shared_messagequeue_data->in == messagequeue->shared_messagequeue_data->out;
}

unsigned int sm_messagequeue_full(sm_messagequeue_t *messagequeue) {
    return sm_messagequeue_length(messagequeue) == messagequeue->shared_messagequeue_data->mask + 1;
}

unsigned int sm_messagequeue_length(sm_messagequeue_t *messagequeue) {
    return messagequeue->shared_messagequeue_data->in - messagequeue->shared_messagequeue_data->out;
}
