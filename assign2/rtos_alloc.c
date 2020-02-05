//
// Created by Benxin Niu on 2020-02-05.
//

#include "rtos_alloc.h"
#include <stdio.h>
#include <sys/mman.h> /* mmap() is defined in this header */

#define ARENA_SIZE 307200

#define HEADER_SIZE 200

struct arena {
    void *range;
    void *start;
    void *chunk_start;
    size_t allocated_size;
    struct chunk *current;
};

struct chunk {
    size_t size;
    void *ptr;
    struct chunk *prev;
    struct chunk *next;
};

void check_or_init_arena();

struct arena *raptors;

void *rtos_malloc(size_t size) {
    check_or_init_arena();
    void *p = mmap(raptors->start, size + HEADER_SIZE, PROT_READ | PROT_WRITE, MAP_ANON | MAP_PRIVATE, -1, 0);
    if (p != NULL) {
        raptors->start = p;
        raptors->allocated_size += size + HEADER_SIZE;
        struct chunk *chunk = mmap(raptors->chunk_start, sizeof(struct chunk), PROT_READ | PROT_WRITE,
                                   MAP_ANON | MAP_PRIVATE, -1, 0);
        raptors->chunk_start = chunk;
        chunk->size = size + HEADER_SIZE;
        chunk->ptr = p;
        chunk->next = NULL;
        chunk->prev = raptors->current;
        raptors->current->next = chunk;
        raptors->current = chunk;
    }
    return p;
}

void *rtos_realloc(void *ptr, size_t size) {
    check_or_init_arena();
    rtos_free(ptr);
    ptr = rtos_malloc(size);
    return ptr;
}

void rtos_free(void *ptr) {
    if (ptr == NULL) {
        return;
    }
    check_or_init_arena();
    size_t size = 0;
    struct chunk *curr = raptors->current;
    if (curr->ptr == ptr) {
        size = curr->size;
        raptors->current = curr->prev;
        raptors->allocated_size -= size;
        raptors->start = raptors->current->ptr;
        raptors->chunk_start = curr->prev;
        munmap(curr, sizeof(struct chunk));
        munmap(ptr, size);
        return;
    }
    curr = raptors->current->prev;
    while (curr != NULL) {
        if (curr->ptr == ptr) {
            size = curr->size;
            curr->next->prev = curr->prev;
            raptors->allocated_size -= size;
            raptors->start = curr->prev->ptr;
            munmap(curr, sizeof(struct chunk));
            munmap(ptr, size);
            break;
        }
        curr = curr->prev;
    }
}

size_t rtos_alloc_size(void *ptr) {
    check_or_init_arena();
    struct chunk *curr = raptors->current;
    if (curr->ptr == ptr) {
        return curr->size;
    }
    while (curr->prev != NULL) {
        if (curr->ptr == ptr) {
            return curr->size;
        }
        curr = curr->prev;
    }
    return 0;
}

bool rtos_allocated(void *ptr) {
    check_or_init_arena();
    struct chunk *curr = raptors->current;
    if (curr->ptr == ptr) {
        return true;
    }
    while (curr->prev != NULL) {
        if (curr->ptr == ptr) {
            return true;
        }
        curr = curr->prev;
    }
    return false;
}

size_t rtos_total_allocated(void) {
    check_or_init_arena();
    return raptors != NULL ? raptors->allocated_size : 0;
}

bool rtos_is_valid(void *ptr) {
    check_or_init_arena();
    if (ptr == NULL)
        return false;
    struct chunk *curr = raptors->current;
    if (curr->ptr == ptr) {
        return true;
    }
    while (curr->prev != NULL) {
        if (curr->ptr == ptr) {
            return true;
        }
        curr = curr->prev;
    }
    return false;
}

void check_or_init_arena() {
    if (raptors == NULL) {
        struct chunk *current = mmap(0, sizeof(struct chunk), PROT_READ | PROT_WRITE, MAP_ANON | MAP_PRIVATE, -1, 0);
        current->next = NULL;
        current->prev = NULL;
        current->ptr = NULL;
        struct arena *ptr = mmap(current, sizeof(struct arena), PROT_READ | PROT_WRITE, MAP_ANON | MAP_PRIVATE, -1, 0);
        ptr->range = mmap(ptr, ARENA_SIZE, PROT_READ | PROT_WRITE, MAP_ANON | MAP_PRIVATE, -1, 0);
        ptr->start = ptr;
        ptr->allocated_size = 0;
        ptr->current = current;
        ptr->chunk_start = ptr->range;
        raptors = ptr;
    }
}