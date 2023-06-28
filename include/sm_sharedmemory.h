//
// Created by my on 2023/4/18.
//

#ifndef SM_RINGBUFFER_SM_SHAREDMEMORY_H
#define SM_RINGBUFFER_SM_SHAREDMEMORY_H

#ifdef __cplusplus
extern "C" {
#endif

#define SM_MAX_SHARED_MEMORY_NAME_LENGTH            256

#define SM_SHAREDMEMORY_OPEN_MODE_CREATE            0
#define SM_SHAREDMEMORY_OPEN_MODE_OPEN              1

typedef struct sm_sharedmemory_t {
  char name[SM_MAX_SHARED_MEMORY_NAME_LENGTH];
  unsigned int mode;
  unsigned int size;
#ifdef WIN32
  void *handle;
#else
  int handle;
#endif
  void *base_address;
} sm_sharedmemory_t;

extern unsigned int sm_sharedmemory_open(sm_sharedmemory_t *sharedmemory, unsigned int mode, const char *name,
                                         unsigned int size);

extern unsigned int sm_sharedmemory_close(sm_sharedmemory_t *sharedmemory);

extern void *sm_sharedmemory_get_base_address(sm_sharedmemory_t *sharedmemory);

#ifdef __cplusplus
}
#endif

#endif //SM_RINGBUFFER_SM_SHAREDMEMORY_H
