#include <string.h> // strcpy
#include <stdio.h>  // printf
#include <stdlib.h> // malloc, free
#include "linked-list.h"
#include "grades.h"

enum {MINGRADE = 0, MAXGRADE = 100};

/* Functions return values */
typedef enum { SUCCESS = 0, FAIL = -1 } Result;

/* Struct defines */
struct grades {
    struct list* students;
};

typedef struct student {
    char* name;
    int id;
    struct list* courses;
} *pstudent_t, student_t;

typedef struct course {
    char* name;
    int grade;
    
} *pcourse_t, course_t;


/*** Static Auxiliary functions, in use only in grades.c ***/
/* DECLARATION ONLY - DOCUMENTATION BELOW */
static pcourse_t find_course(struct list* courses, const char *name);
static pstudent_t find_student(struct list* students, int id);
static char *alter_strdup(const char *c);

/*** USER FUNCTIONS ***/
/* Courses LL User-Functions */

int course_clone_fn(void *course, void **output) {
    // Allocate memory for the clone
    pcourse_t course_clone = (pcourse_t)malloc(sizeof(pcourse_t));
    if (!course_clone) {
        return FAIL;
    }
    // Cast to course type and set values
    pcourse_t _course = (pcourse_t)course;
    course_clone->name = alter_strdup(_course->name);
    if (!course_clone->name){
        return FAIL;
    }
    course_clone->grade = _course->grade;

    *output = course_clone;
    return SUCCESS;

}

void course_destroy_fn(void* course) {
    // Cast input argument to operate on it accordingly
    pcourse_t _course = (pcourse_t)course;
    // Free all related memory
    free(_course->name);
    free(_course);
}



/*** Student LL User-Functions ***/

int student_clone_fn(void *student, void **output) {

    // Allocate memory for the clone
    pstudent_t clone_student = (pstudent_t)malloc(sizeof(pstudent_t));
    if (!clone_student) {
        return FAIL;
    }
    // Cast input to student type using a "dummy temp variable"
    pstudent_t temp_student = (pstudent_t)student;
    pcourse_t temp_course = NULL; // Used for building a clone list

    // Set values
    clone_student->name = alter_strdup(temp_student->name);
    if (!clone_student->name) {
        return FAIL;
    }
    clone_student->id=temp_student->id;

    // Create a new copy of the courses list of the original student.
    struct list* courses_list = list_init(course_clone_fn,course_destroy_fn);
    if (!courses_list) {
        return FAIL;
    }

    // Set an iterator to traverse the courses list
    struct iterator* current_course = list_begin(temp_student->courses);
    while (NULL != current_course) {
        temp_course = list_get(current_course);
        if(NULL != current_course){
            list_push_back(courses_list, temp_course);
        }
        current_course = list_next(current_course);
    }

    clone_student->courses = courses_list;
    *output = clone_student;
    return SUCCESS;
}


void student_destroy_fn(void *student) {
    // Cast input argument to operate on it accordingly
    pstudent_t _student = (pstudent_t)student;
    // Free all related memory
    free(_student->name);
    list_destroy(_student->courses);
    free(_student);
}



/***  LIBGRADES FUNCTIONS  ***/

struct grades* grades_init() {
    
    struct grades* new_grades = (struct grades*)malloc(sizeof(struct grades*));
    // Initialize a students list
    struct list* student_list = list_init(student_clone_fn,
                                           student_destroy_fn);
    new_grades->students = student_list;

    return new_grades;
}



void grades_destroy(struct grades *grades) {
    // Invalid grades struct given to the function => ERROR
    if (!grades) {
        return;
    }
    // Empty students list => just free the grades struct.
    else if (!(grades->students)) {
        free(grades);
        return;
    }
    // Otherwise free the memory both grades and it's students list.
    list_destroy(grades->students);
    free(grades);
}



int grades_add_student(struct grades *grades, const char *name, int id) {
    // Invalid grades struct given to the function => ERROR
    if (NULL == grades) {
        return FAIL;
    }
    // If a student with the same id already exists in the list => ERROR
    if (NULL != find_student(grades->students, id)) {
        return FAIL;
    }
    // Build the student to be added, set all values
    pstudent_t new_student = (pstudent_t)malloc(sizeof(pstudent_t));
    new_student->name = alter_strdup(name);
    new_student->id = id;
    new_student->courses = list_init(course_clone_fn, course_destroy_fn);

    // Push a copy of the new student to the students list
    if (list_push_back(grades->students, new_student)) {
        return FAIL; // Failed to push a new element to the students list.
    }
    // Deallocate ALL temp memory of the new_student
    free(new_student->name);
    list_destroy(new_student->courses);
    free(new_student);

    return SUCCESS;
}



int grades_add_grade(struct grades *grades, const char *name,
int id, int grade){
    // Invalid grades struct given to the function => ERROR
    if (NULL == grades) {
        return FAIL;
    }
    pstudent_t student = find_student(grades->students, id);
     // Check that a student with the same id actually exists in the list.
    if (NULL == student) {
        return FAIL;
    }
    // Check for invalid grade.
    if (grade > MAXGRADE || grade < MINGRADE) {
        return FAIL;
    }
    // Check if a course with the same name already exists.
    if (NULL != find_course(student->courses, name)) {
        return FAIL;
    }
    course_t course;
    course.grade = grade;
    course.name = alter_strdup(name);
    if (!course.name) {
        return FAIL;
    }
    pcourse_t course_pointer = &course;
    if (list_push_back(student->courses, course_pointer)) {
        return FAIL; // Failed to push a new course to the list.
    }
    free(course.name);
    return SUCCESS;           
}



float grades_calc_avg(struct grades *grades, int id, char **out){
    // Invalid grades struct given to the function => ERROR
    if (NULL == grades) {
        *out = NULL;
        return FAIL;
        
    }

    pstudent_t student = find_student(grades->students, id);
     // Check that a student with the same id actually exists in the list.
    if (NULL == student) {
        *out = NULL;
        return FAIL;
    }
    // Assign student's name to 'out' paramater
    *out = alter_strdup(student->name);
    if (!*out) {
        return FAIL;
    }
    
    // Student has no courses => avg = 0
    if (list_size(student->courses) == 0) {
        return MINGRADE;
    }

    float avg = 0;
    int counter = 0;

    struct iterator* it = list_begin(student->courses);
    if (!it) {
        *out = NULL;
        return FAIL;
    }
    // Traverse the courses list
    while (NULL != it) {

        pcourse_t current_course = list_get(it);
        avg += current_course->grade;
        counter++;
        it = list_next(it);
    }

    // Calc the average.
    avg = avg/counter;
    return avg;
}


int grades_print_student(struct grades *grades, int id) {
    // Invalid grades struct given to the function => ERROR
    if (NULL == grades) {
        return FAIL;
    }

     pstudent_t student = find_student(grades->students, id);
     // Check that a student with the same id actually exists in the list.
    if (NULL == student) {
        return FAIL;
    }

    struct iterator* it = list_begin(student->courses);
    pcourse_t course = NULL;

    printf("%s %d:", student->name, student->id);
    // Traverse the student's courses list and print them + their grades
    while (NULL != it) {

        course = list_get(it);
        it = list_next(it);
        if (!it) {
            printf(" %s %d", course->name, course->grade);
        }
        else {
            printf(" %s %d,", course->name, course->grade);
        } 
    }
    printf("\n");
    return SUCCESS;
}



int grades_print_all(struct grades *grades) {
    // Invalid grades struct given to the function => ERROR
    if (NULL == grades) {
        return FAIL;
    }
    // Check if the list is empty
    if (list_size(grades->students) == 0) {
       return SUCCESS;
    }
    
    struct iterator* it = list_begin(grades->students);
    if (!it) {
        return FAIL;
    }

    pstudent_t student = NULL; // Dummy temp variable
    // Traverse the list and operate on each student
    while (NULL != it) {

        student = list_get(it);
        grades_print_student(grades, student->id);
        it = list_next(it);
    }
    return SUCCESS;
}




/*** IMPLEMANTAION OF AUXILIARY FUNCTIONS ***/

static pstudent_t find_student(struct list* students, int id) {

    struct iterator* it = list_begin(students);
    pstudent_t current_student = NULL;

    // Scans the list from the start, search for a student with the correct id.
    while (NULL != it) {
        current_student = list_get(it);
        if (current_student->id == id) {
            return current_student; // Return pointer to student if found
        }
        it = list_next(it);
    }

    // Student was not found.
    return NULL; 
}


static pcourse_t find_course(struct list* courses, const char *name) {

    struct iterator* it = list_begin(courses);
    pcourse_t current_course = NULL;

    // Scans the list from the start, search for a course with the correct name.
    while (NULL != it) {
        current_course = list_get(it);
        if (!strcmp(current_course->name,name)) {
            return current_course; // Return pointer to course if found
        }
        it = list_next(it);
    }

    // Course was not found.
    return NULL; 
}


static char *alter_strdup(const char *c)
{
    // Allocate memory for the copy
    char *dup = malloc(strlen(c) + 1);

    if (dup != NULL) {
       strcpy(dup, c);
    }
    // Return a pointer to the newly created string - which is a copy of 'c'
    return dup;
}
