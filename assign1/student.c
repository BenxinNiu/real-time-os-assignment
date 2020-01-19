//
// Created by Benxin Niu on 2020-01-18.
//

#include <stdio.h>
#include <stdlib.h>
#include "student.h"
#include "course.h"

#define MAX_NUM_COURSE 6

struct course {
    enum subject subject;
    int refcount;
    uint16_t code;
    double grade;
};

struct student {
    struct student_id id;
    bool is_grad;
    struct course **registered_courses;
    int num_of_registered;
};

void init_student_course(struct student *stu);

bool is_course_pass(struct course *course, bool is_grad);

struct course *take_course(struct course *course, uint8_t grade);

struct student *student_create(struct student_id id, bool grad_student) {
    struct student *stu;
    stu = malloc(sizeof(struct student));
    stu->id = id;
    stu->is_grad = grad_student;
    init_student_course(stu);
    return stu;
}

void student_free(struct student *student) {
    free(student);
}

void student_take(struct student *s, struct course *course, uint8_t grade) {
    if (s->num_of_registered < MAX_NUM_COURSE) {
        course_hold(course);
        s->registered_courses[s->num_of_registered] = take_course(course, grade);
        (s->num_of_registered)++;
    } else {
        printf("Too much work is no good for anyone... it's Term 8 baby!\n");
    }
}

int student_grade(struct student *student, struct course *course) {
    for (int i = 0; i < student->num_of_registered; i++) {
        if (student->registered_courses[i]->code == course->code &&
            student->registered_courses[i]->subject == course->subject) {
            return student->registered_courses[i]->grade;
        }
    }
    return -1;
}

double student_passed_average(const struct student *student) {
    double subtotal = 0.0;
    int num_of_pass = 0;
    for (int i = 0; i < student->num_of_registered; i++) {
        if (is_course_pass(student->registered_courses[i], student->is_grad)) {
            subtotal += student->registered_courses[i]->grade;
            num_of_pass++;
        }
    }
    return num_of_pass == 0 ? 0 : subtotal / num_of_pass;
}

bool student_promotable(const struct student *student) {
    double cumulative = 0;
    int num_of_failed = 0;
    for (int i = 0; i < student->num_of_registered; i++) {
        cumulative += student->registered_courses[i]->grade;
        if (student->is_grad && student->registered_courses[i]->grade < 65) {
            num_of_failed++;
        }
    }
    return student->is_grad ?
           num_of_failed <= 1 && student->num_of_registered > 1 :
           (cumulative / student->num_of_registered) >= 60;
}

void init_student_course(struct student *stu) {
    stu->num_of_registered = 0;
    stu->registered_courses = malloc(sizeof(struct course *) * MAX_NUM_COURSE);
    for (int i = 0; i < MAX_NUM_COURSE; i++) {
        stu->registered_courses[i] = malloc(sizeof(struct course));
    }
}

bool is_course_pass(struct course *course, bool is_grad) {
    return is_grad ? course->grade >= 65 : course->grade >= 50;
}

struct course *take_course(struct course *course, uint8_t grade) {
    struct course *registered_course;
    registered_course = malloc(sizeof(struct course));
    registered_course->subject = course->subject;
    registered_course->code = course->code;
    registered_course->refcount = course->refcount;
    registered_course->grade = grade;
    return registered_course;
}