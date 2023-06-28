#ifndef SM_RINGBUFFER_SM_RINGBUFFER_H
#define SM_RINGBUFFER_SM_RINGBUFFER_H

#ifdef __cplusplus
extern "C" {
#endif

typedef struct sm_shared_ringbuffer_data_t {
  unsigned int in;
  unsigned int out;
  unsigned int mask;
  unsigned int element_size;
} sm_shared_ringbuffer_data_t;

typedef struct sm_ringbuffer_t {
  void *base_address;
  sm_shared_ringbuffer_data_t *shared_ringbuffer_data;
} sm_ringbuffer_t;

extern unsigned int sm_ringbuffer_init(sm_ringbuffer_t *ringbuffer, sm_shared_ringbuffer_data_t *shared_ringbuffer_data,
                                       void *buffer, unsigned int buffer_size,
                                       unsigned int element_size);

extern unsigned int sm_ringbuffer_open(sm_ringbuffer_t *ringbuffer, sm_shared_ringbuffer_data_t *shared_ringbuffer_data,
                                       void *base_address);

extern unsigned int sm_ringbuffer_peek_in_place(sm_ringbuffer_t *ringbuffer, void **data);

extern unsigned int sm_ringbuffer_skip_one(sm_ringbuffer_t *ringbuffer);

extern unsigned int sm_ringbuffer_poke_in_place(sm_ringbuffer_t *ringbuffer, void **data);

extern unsigned int sm_ringbuffer_push_in_place(sm_ringbuffer_t *ringbuffer);

extern unsigned int sm_ringbuffer_push(sm_ringbuffer_t *ringbuffer, void *data);

extern unsigned int sm_ringbuffer_pop(sm_ringbuffer_t *ringbuffer, void *data);

extern unsigned int sm_ringbuffer_empty(sm_ringbuffer_t *ringbuffer);

extern unsigned int sm_ringbuffer_full(sm_ringbuffer_t *ringbuffer);

extern unsigned int sm_ringbuffer_length(sm_ringbuffer_t *ringbuffer);

extern unsigned int sm_ringbuffer_capacity(sm_ringbuffer_t *ringbuffer);

typedef struct sm_shared_messagequeue_data_t {
  unsigned int in;
  unsigned int out;
  unsigned int mask;
} sm_shared_messagequeue_data_t;

typedef struct sm_messagequeue_t {
  void *base_address;
  sm_shared_messagequeue_data_t *shared_messagequeue_data;
} sm_messagequeue_t;

extern unsigned int sm_messagequeue_init(sm_messagequeue_t *messagequeue,
                                         sm_shared_messagequeue_data_t *shared_messagequeue_data,
                                         void *buffer, unsigned int buffer_size);

extern unsigned int sm_messagequeue_open(sm_messagequeue_t *messagequeue,
                                         sm_shared_messagequeue_data_t *shared_messagequeue_data,
                                         void *base_address);

extern unsigned int sm_messagequeue_peek_in_place(sm_messagequeue_t *messagequeue, void **data,
                                                  unsigned int *element_size);

extern unsigned int sm_messagequeue_skip_one(sm_messagequeue_t *messagequeue);

extern unsigned int sm_messagequeue_poke_in_place(sm_messagequeue_t *messagequeue, void **data,
                                                  unsigned int element_size);

extern unsigned int sm_messagequeue_push_in_place(sm_messagequeue_t *messagequeue);

extern unsigned int sm_messagequeue_push(sm_messagequeue_t *messagequeue, void *data, unsigned int element_size);

extern unsigned int sm_messagequeue_pop(sm_messagequeue_t *messagequeue, void *data, unsigned int *element_size);

extern unsigned int sm_messagequeue_empty(sm_messagequeue_t *messagequeue);

extern unsigned int sm_messagequeue_full(sm_messagequeue_t *messagequeue);

extern unsigned int sm_messagequeue_length(sm_messagequeue_t *messagequeue);

#ifdef __cplusplus
}
#endif

#endif //SM_RINGBUFFER_SM_RINGBUFFER_H
