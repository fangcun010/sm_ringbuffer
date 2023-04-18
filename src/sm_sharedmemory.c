#include <sm_sharedmemory.h>

#include <string.h>

#ifdef WIN32

#include <windows.h>

#endif

unsigned int sm_sharedmemory_open(sm_sharedmemory_t *sharedmemory, unsigned int mode, const char *name,
                                  unsigned int size) {
#ifdef WIN32
    HANDLE handle;

    if (mode == SM_SHAREDMEMORY_OPEN_MODE_CREATE)
        handle = CreateFileMapping(INVALID_HANDLE_VALUE, NULL, PAGE_READWRITE, 0, size, name);
    else if (mode == SM_SHAREDMEMORY_OPEN_MODE_OPEN)
        handle = OpenFileMapping(FILE_MAP_ALL_ACCESS, FALSE, name);
    else
        return 0;
#else
#endif

#ifdef WIN32
    if (!handle)
        return 0;

    void *base_address = MapViewOfFile(handle, FILE_MAP_ALL_ACCESS, 0, 0, size);

    if (!base_address)
        return 0;

    strcpy(sharedmemory->name, name);
    sharedmemory->mode = mode;
    sharedmemory->handle = handle;
    sharedmemory->size = size;
    sharedmemory->base_address = base_address;
#else
#endif

    return 1;
}

unsigned int sm_sharedmemory_close(sm_sharedmemory_t *sharedmemory) {
#ifdef WIN32
    UnmapViewOfFile(sharedmemory->base_address);
    CloseHandle(sharedmemory->handle);
#else
#endif
    return 1;
}

void *sm_sharedmemory_get_base_address(sm_sharedmemory_t *sharedmemory) {
    return sharedmemory->base_address;
}