#ifndef SM_RINGBUFFER_SM_RINGBUFFER_H
#define SM_RINGBUFFER_SM_RINGBUFFER_H

typedef struct sm_ringbuffer_t {
    unsigned int in;
    unsigned int out;
    unsigned int mask;
    unsigned int element_size;
    void *data;
} sm_ringbuffer_t;

extern unsigned int sm_ringbuffer_init(sm_ringbuffer_t *ringbuffer, void *buffer, unsigned int buffer_size,
                                       unsigned int element_size);

extern unsigned int sm_ringbuffer_push(sm_ringbuffer_t *ringbuffer, void *data);

extern unsigned int sm_ringbuffer_pop(sm_ringbuffer_t *ringbuffer, void *data);

extern unsigned int sm_ringbuffer_empty(sm_ringbuffer_t *ringbuffer);

extern unsigned int sm_ringbuffer_full(sm_ringbuffer_t *ringbuffer);

extern unsigned int sm_ringbuffer_length(sm_ringbuffer_t *ringbuffer);


#endif //SM_RINGBUFFER_SM_RINGBUFFER_H
