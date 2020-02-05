#include <stdio.h>
#include <time.h>

#include "rtos_alloc.h"
#include <err.h>

#include "assert.h"

void valid_allocation() {
    size_t s = 8;
    void *p = rtos_malloc(s);
    size_t size = rtos_alloc_size(p);
    bool is_valid = rtos_is_valid(p);
    assert(is_valid);
    assert(size>=s);
}

void basic_allocation() {
    void *p = rtos_malloc(200);
    assert(p != NULL);
}

void multiple_allocation() {
    size_t total = 0;
    size_t s[6] = { 1, 2, 17, 42, 1049, 50 };
    void *pointers[6];
    size_t before = rtos_total_allocated();
    for (int i= 0; i < 6; i++)
    {
        void *p = rtos_malloc(s[i]);
        size_t size = rtos_alloc_size(p);
        assert(size == 200+s[i]);
        total += size;
        pointers[i] = p;

    }
    assert(total==rtos_total_allocated()-before);
    for (int i= 0; i < 6; i++){
        rtos_free(pointers[i]);
    }
    assert(before==rtos_total_allocated());
}

void validate_allocations(){
    void *p_one = rtos_malloc(20);
    void *p_two = rtos_malloc(15);
    void *p_bad = malloc(30);

    assert(rtos_is_valid(p_one));
    assert(rtos_allocated(p_one));

    assert(rtos_is_valid(p_two));
    assert(rtos_allocated(p_two));

    assert(!rtos_is_valid(p_bad));
    assert(!rtos_allocated(p_bad));
}

void validate_free() {
    void *p_one = rtos_malloc(20);
    void *p_bad = NULL;

    assert(rtos_is_valid(p_one));
    assert(rtos_allocated(p_one));

    rtos_free(p_one);
    rtos_free(p_bad);

    assert(!rtos_is_valid(p_bad));
    assert(!rtos_is_valid(p_one));
    assert(!rtos_allocated(p_one));
}

long get_time_for_allocation(size_t size, bool is_malloc){
    struct timespec begin, end;
    if (clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &begin) != 0)
    {
        err(-1, "Failed to get start time");
    }

    void *p = is_malloc ? malloc(size) : rtos_malloc(size);

    if (clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &end) != 0)
    {
        err(-1, "Failed to get end time");
    }
    return end.tv_nsec - begin.tv_nsec;
//    printf("\nTotal time %ld nano seconds \n", elapsed);
}

int main() {
    // Test
    validate_allocations();
    valid_allocation();
    basic_allocation();
    multiple_allocation();
    validate_free();

    // Test performance
    int test_size = 22;

    size_t s[22] = { 1, 2, 17, 42, 1049, 50, 1049, 20, 30, 7, 42, 50 ,10, 62, 19, 42, 2049, 50, 102400, 20, 122400, 132400};

    long my_allocation_time[22];
    long malloc_allocation_time[22];

    for (int i=0; i<test_size; i++){
        my_allocation_time[i] = get_time_for_allocation(s[i], false);
        malloc_allocation_time[i] = get_time_for_allocation(s[i], true);

        printf("For size of %zu bytes allocation, my rtos_malloc spent: %ld vs malloc: %ld \n", s[i], my_allocation_time[i], malloc_allocation_time[i]);
    }

    return 0;
}