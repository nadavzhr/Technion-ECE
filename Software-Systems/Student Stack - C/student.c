#include "student.h"
#include <string.h> // strlen, strcpy
#include <stdlib.h> // malloc, free
#include <stdio.h>  // fprintf
/**
 * @fn *strdup_alt(const char *str)
 * @brief Creates a duplicate of the given string
 * @param char *str is the string we want to duplicate.
 * @return is a pointer to the cloned string.
*/
static char *strdup_alt(const char *str) {
    size_t len = strlen(str) + 1; // add 1 for the null terminator
    char *new_str = malloc(len); // allocate memory for the new string
    if (new_str == NULL) { // check if the allocation was successful
        return NULL;
    }
    strcpy(new_str, str); // copy the string into the new memory location
    return new_str;
}

elem_t student_clone(elem_t student) {
    /* Build the clone */
    pstudent_t clone_student = malloc(sizeof(student_t));
    if(NULL == clone_student) {
        fprintf(stderr, "Failed to allocate memory.\n");
        return NULL;
    }
    pstudent_t _student = (pstudent_t)student;

    /* Check for errors */
    if (NULL == _student->name) {
        fprintf(stderr, "Invalid student name.\n");
        free(clone_student);
        return NULL;
    }
    else if (_student->age < 0) {
        fprintf(stderr, "Invalid student age.\n");
        free(clone_student);
        return NULL;
    }
    else if (_student->id < 0) {
        fprintf(stderr, "Invalid student ID.\n");
        free(clone_student);
        return NULL;
    }

    /* No errors, set fields */
    clone_student->age = _student->age;
    clone_student->id = _student->id;
    clone_student->name = strdup_alt(_student->name);
    if (NULL == clone_student->name) {
        free(clone_student);
        return NULL;
    }
    
    return (elem_t)clone_student;
}



void student_destroy(elem_t student) {
    pstudent_t _student = (pstudent_t)student;
    free(_student->name);
    free(_student);
}



void student_print(elem_t student) {

    if (NULL == student) {
        fprintf(stderr, "No student found.\n");
        return;
    }
    pstudent_t _student = (pstudent_t)student;

    // No errors, set values accoridngly
    char* name = _student->name;
    int age = _student->age;
    int id = _student->id;

    printf("student name: %s, age: %d, id: %d.\n", name, age, id);
}
