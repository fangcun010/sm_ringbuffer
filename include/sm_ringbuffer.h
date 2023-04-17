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

typedef struct sm_messagequeue_t {
    unsigned int in;
    unsigned int out;
    unsigned int mask;
    void *data;
} sm_messagequeue_t;

extern unsigned int sm_messagequeue_init(sm_messagequeue_t *messagequeue, void *buffer, unsigned int buffer_size);

extern unsigned int sm_messagequeue_push(sm_messagequeue_t *messagequeue, void *data,unsigned int element_size);

extern unsigned int sm_messagequeue_pop(sm_messagequeue_t *messagequeue, void *data,unsigned int *element_size);

extern unsigned int sm_messagequeue_empty(sm_messagequeue_t *messagequeue);

extern unsigned int sm_messagequeue_full(sm_messagequeue_t *messagequeue);

extern unsigned int sm_messagequeue_length(sm_messagequeue_t *messagequeue);

#endif //SM_RINGBUFFER_SM_RINGBUFFER_H
