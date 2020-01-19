#include <stdio.h>
#include "course.h"
#include "student.h"

struct course {
    enum subject subject;
    int refcount;
    uint16_t code;
};

//int main() {
//    printf("Hello, World!\n");
//    struct course *course = course_create(SUBJ_ECE, 8899);
//    printf("\n%d\n", course_subject(course));
//    printf("\n%d\n", course_code(course));
//
//    printf("\n%d\n", course->refcount);
//    printf("\n%d\n", course->refcount);
//
//    course_hold(course);
//    course_hold(course);
//    course_hold(course);
//
//    printf("\n%d\n", course->refcount);
//    printf("\n%d\n", course->refcount);
//
//    course_release(course);
//
//    printf("\n%d\n", course->refcount);
//    printf("\n%d\n", course->refcount);
//    return 0;
//}

int main() {
    printf("Hello, World!\n");
    struct course *course;
    struct course *course_two;
    struct course *course_three;
    course = course_create(SUBJ_ECE, 8899);
    course_two = course_create(SUBJ_CIV, 8899);
    course_three = course_create(SUBJ_CHEM, 6677);

    struct student_id id = {2015, 123};
    struct student *undergrad = student_create(id, false);
    struct student *grad = student_create(id, true);

    student_take(grad, course, 64);
    student_take(grad, course_two, 65);

    student_take(undergrad, course, 64);
    student_take(undergrad, course_two, 60);
    student_take(undergrad, course_three, 60);

    int grade = student_grade(grad, course_two);
    printf("\nstudent grade : %d\n", grade);

    double average = student_passed_average(grad);
    printf("\nstudent average : %f\n", average);

    bool promoted = student_promotable(grad);
    if (promoted)
        printf("\nPROMOTED\n");
    else
        printf("\nNOT PROMOTED\n");

    printf("\ncourse been taken : %d times\n", course->refcount);

}
