//
// Created by fangcun on 23-4-15.
//
#include <stdio.h>

#include <sm_ringbuffer.h>

#define BUFFER_SIZE 16
#define ELEMENT_SIZE 4

char buffer[BUFFER_SIZE];
unsigned int element;

int main(int argc, char *argv[]) {
    sm_ringbuffer_t ringbuffer;

    sm_ringbuffer_init(&ringbuffer, buffer, BUFFER_SIZE, ELEMENT_SIZE);

    for (int i = 0; i < 5; i++) {
        element = i + 1;
        sm_ringbuffer_push(&ringbuffer, &element);
    }

    unsigned int out_val;

    int len = sm_ringbuffer_length(&ringbuffer);

    while (!sm_ringbuffer_empty(&ringbuffer)) {
        sm_ringbuffer_pop(&ringbuffer, &out_val);
        printf("%d\n", out_val);
    }

    return 0;
}