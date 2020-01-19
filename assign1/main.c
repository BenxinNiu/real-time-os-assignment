#include <stdio.h>
#include "course.h"
#include "student.h"
#include <assert.h>

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

void test_student_creation() {
    // Arrange
    struct student_id id_one = {2015, 88997766};
    struct student_id id_two = {2018, 12345678};

    // Act
    struct student *under_grad = student_create(id_one, false);
    struct student *graduate = student_create(id_two, true);

    // Assert
    assert(under_grad->num_of_registered == 0);
    assert(under_grad->is_grad == false);
    assert(under_grad->id.sid_serial == 88997766);
    assert(under_grad->id.sid_year == 2015);

    assert(graduate->num_of_registered == 0);
    assert(graduate->is_grad == true);
    assert(graduate->id.sid_serial == 12345678);
    assert(graduate->id.sid_year == 2018);
}

void test_student_free() {
    // Arrange
    struct student_id id_one = {2015, 88997766};
    struct student *under_grad = student_create(id_one, false);

    // Act
    student_free(under_grad);

    // Nothing to assert as unexpected outcome
}

void test_student_take() {
    // Arrange
    struct student_id id_one = {2015, 88997766};
    struct student_id id_two = {2018, 12345678};

    struct student *under_grad = student_create(id_one, false);
    struct student *graduate = student_create(id_two, true);

    struct course *course_one = course_create(SUBJ_ECE, 8899);
    struct course *course_two = course_create(SUBJ_MATH, 2233);
    struct course *course_three = course_create(SUBJ_MATH, 1234);


    // Act
    student_take(graduate, course_one, 64);
    student_take(graduate, course_two, 65);

    student_take(under_grad, course_one, 64);
    student_take(under_grad, course_two, 60);
    student_take(under_grad, course_three, 61);

    // Assert
    assert(graduate->num_of_registered == 2);
    assert(under_grad->num_of_registered == 3);
    assert(course_one->refcount == 3);
    assert(course_two->refcount == 3);
    assert(course_three->refcount == 2);

    assert(under_grad->registered_courses[0]->grade == 64);
    assert(under_grad->registered_courses[1]->grade == 60);
    assert(under_grad->registered_courses[2]->grade == 61);
    assert(graduate->registered_courses[0]->grade == 64);
    assert(graduate->registered_courses[1]->grade == 65);
}

void test_student_grade() {
    // Arrange
    struct student_id id_one = {2015, 88997766};
    struct student_id id_two = {2018, 12345678};

    struct student *under_grad = student_create(id_one, false);
    struct student *graduate = student_create(id_two, true);

    struct course *course_one = course_create(SUBJ_ECE, 8899);
    struct course *course_two = course_create(SUBJ_MATH, 2233);

    student_take(under_grad, course_one, 64);
    student_take(graduate, course_two, 65);

    // Act
    int undergrad_course_one_grade = student_grade(under_grad, course_one);
    int undergrad_course_two_grade = student_grade(under_grad, course_two);

    int grad_course_one_grade = student_grade(graduate, course_one);
    int grad_course_two_grade = student_grade(graduate, course_two);

    // Assert
    assert(undergrad_course_one_grade == 64);
    assert(undergrad_course_two_grade == -1);
    assert(grad_course_one_grade == -1);
    assert(grad_course_two_grade == 65);
}

void test_student_passed_average() {
    // Arrange
    struct student_id id_one = {2015, 88997766};
    struct student_id id_two = {2018, 12345678};
    struct student_id id_three = {2017, 987654321};

    struct student *under_grad = student_create(id_one, false);
    struct student *graduate = student_create(id_two, true);
    struct student *bad_under_grad = student_create(id_three, false);

    struct course *course_one = course_create(SUBJ_ECE, 8899);
    struct course *course_two = course_create(SUBJ_CIV, 2233);
    struct course *course_three = course_create(SUBJ_MATH, 1234);

    student_take(graduate, course_one, 64);
    student_take(graduate, course_two, 65);
    student_take(graduate, course_three, 70);

    student_take(under_grad, course_one, 64);
    student_take(under_grad, course_two, 49);
    student_take(under_grad, course_three, 50);

    student_take(bad_under_grad, course_two, 49);
    student_take(bad_under_grad, course_three, 49);

    // Act
    double bad_under_grad_avg = student_passed_average(bad_under_grad);
    double under_grad_avg = student_passed_average(under_grad);
    double graduate_avg = student_passed_average(graduate);

    // Assert
    assert(bad_under_grad_avg == 0);
    assert(under_grad_avg == 57);
    assert(graduate_avg == 67.5);
}

void test_student_promotable() {
    // Arrange
    struct student_id id_one = {2015, 88997766};
    struct student_id id_two = {2018, 12345678};
    struct student_id id_three = {2017, 987654321};

    struct student *promotable_under_grad = student_create(id_one, false);
    struct student *failed_under_grad = student_create(id_one, false);

    struct student *promotable_graduate_all_pass = student_create(id_two, true);
    struct student *failed_graduate_failed_two = student_create(id_three, true);
    struct student *failed_graduate_failed_one = student_create(id_three, true);
    struct student *promotable_graduate_failed_one = student_create(id_three, true);

    struct course *course_one = course_create(SUBJ_ECE, 8899);
    struct course *course_two = course_create(SUBJ_CIV, 2233);
    struct course *course_three = course_create(SUBJ_MATH, 1234);

    // this grad passed all courses
    student_take(promotable_graduate_all_pass, course_one, 65);
    student_take(promotable_graduate_all_pass, course_two, 65);
    student_take(promotable_graduate_all_pass, course_three, 70);

    // this grad failed two courses
    student_take(failed_graduate_failed_two, course_one, 64);
    student_take(failed_graduate_failed_two, course_two, 64);
    student_take(failed_graduate_failed_two, course_three, 99);

    // this grad only failed one course but only took one course
    student_take(failed_graduate_failed_one, course_one, 64);

    // this grad only failed one course
    student_take(promotable_graduate_failed_one, course_one, 64);
    student_take(promotable_graduate_failed_one, course_two, 65);
    student_take(promotable_graduate_failed_one, course_three, 70);

    //this undergrad cumulative avg is higher than 60
    student_take(promotable_under_grad, course_one, 64);
    student_take(promotable_under_grad, course_two, 49);
    student_take(promotable_under_grad, course_three, 90);

    //this undergrad cumulative avg is lower than 60
    student_take(failed_under_grad, course_two, 59);
    student_take(failed_under_grad, course_three, 59);

    // Act
    bool is_promoted_failed_under_grad = student_promotable(failed_under_grad);
    bool is_promoted_promotable_under_grad = student_promotable(promotable_under_grad);
    bool is_promoted_promotable_graduate_failed_one = student_promotable(promotable_graduate_failed_one);
    bool is_promoted_failed_graduate_failed_one = student_promotable(failed_graduate_failed_one);
    bool is_promoted_failed_graduate_failed_two = student_promotable(failed_graduate_failed_two);
    bool is_promoted_promotable_graduate_all_pass = student_promotable(promotable_graduate_all_pass);

    // Assert
    assert(is_promoted_failed_under_grad == false);
    assert(is_promoted_promotable_under_grad == true);
    assert(is_promoted_promotable_graduate_failed_one == true);
    assert(is_promoted_failed_graduate_failed_one == false);
    assert(is_promoted_failed_graduate_failed_two == false);
    assert(is_promoted_promotable_graduate_all_pass == true);
}

void test_course_creation() {
    // Act
    struct course *course_one = course_create(SUBJ_ECE, 8899);

    // Assert
    assert(course_one->refcount == 1);
    assert(course_one->code == 8899);
    assert(course_one->subject == SUBJ_ECE);
}

void test_course_subject() {
    // Arrange
    struct course *course_one = course_create(SUBJ_ECE, 8899);
    struct course *course_two = course_create(SUBJ_MATH, 2233);

    // Assert
    assert(course_subject(course_one) == 2);
    assert(course_subject(course_two) == 8);
}

void test_course_refcount() {
    // Arrange
    struct course *course_one = course_create(SUBJ_ECE, 8899);
    struct course *course_two = course_create(SUBJ_MATH, 2233);

    // Act
    (course_one->refcount)++;

    // Assert
    assert(course_refcount(course_one) == 2);
    assert(course_refcount(course_two) == 1);
}

void test_course_code() {
    // Arrange
    struct course *course_one = course_create(SUBJ_ECE, 8899);
    struct course *course_two = course_create(SUBJ_MATH, 2233);

    // Act & Assert
    assert(course_code(course_one) == 8899);
    assert(course_code(course_two) == 2233);
}

void test_course_hold_and_release() {
    // Arrange
    struct course *course_one = course_create(SUBJ_ECE, 8899);
    struct course *course_two = course_create(SUBJ_MATH, 2233);

    // Act
    course_hold(course_one);
    course_hold(course_one);

    course_hold(course_two);
    course_hold(course_two);
    course_release(course_two);

    // Assert
    assert(course_one->refcount == 3);
    assert(course_one->code == 8899);
    assert(course_one->subject == SUBJ_ECE);

    assert(course_two->refcount == 2);
    assert(course_two->code == 2233);
    assert(course_two->subject == SUBJ_MATH);

    // Act
    course_release(course_one);
    course_release(course_one);
    course_hold(course_two);

    // Assert
    assert(course_one->refcount == 1);
    assert(course_two->refcount == 3);
}


int main() {
    printf("TDD is the best!!!!\n");

    test_course_creation();
    test_course_hold_and_release();
    test_course_refcount();
    test_course_code();
    test_course_subject();

    test_student_creation();
    test_student_free();
    test_student_take();
    test_student_grade();
    test_student_passed_average();
    test_student_promotable();

}
