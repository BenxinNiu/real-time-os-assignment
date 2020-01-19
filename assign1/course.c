//
// Created by Benxin Niu on 2020-01-18.
//

#include "course.h"
#include <stdio.h>
#include <stdlib.h>

struct course {
    enum subject subject;
    int refcount;
    uint16_t code;
};

struct course *course_create(enum subject subject, uint16_t code) {
    struct course *ptr;
    ptr = malloc(sizeof(struct course));
    ptr->refcount = 1;
    ptr->subject = subject;
    ptr->code = code;
    return ptr;
}

enum subject course_subject(const struct course *course) {
    return course->subject;
}

uint16_t course_code(const struct course *course) {
    return course->code;
}

void course_hold(struct course *course) {
    (course->refcount)++;
}

void course_release(struct course *course) {
    (course->refcount)--;
}

int course_refcount(const struct course *course) {
    return course->refcount;
}