#ifndef __STUDENT__
#define __STUDENT__

#include "stack.h"
#include "stdlib.h"

typedef struct student {
	char *name;
	int age;
    int id;
} *pstudent_t, student_t;


/**
 * @fn elem_t student_clone(elem_t e)
 * @brief Given a pointer to a student struct, it creates a copy of it
 * using memory allocation, with all fields.
 * Remember to free the memory at the end of use.
 * @param e pointer to the student object (a struct).
 * @return a pointer to the copy of the student that was created.
*/
elem_t student_clone(elem_t e);



/**
 * @fn student_destroy(elem_t student)
 * @brief Destroys the student object given to the function.
 * @param student a pointer to the student object to be destroyed.
 * Frees all the memory that was allocated for the given student.
 * @return void
*/
void student_destroy(elem_t student);



/**
 * @fn student_print(elem_t student)
 * @brief Given a student, prints to stdout the student's name, age and ID.
 * @param student pointer to the student object.
 * @return void
*/
void student_print(elem_t student);



#endif /* __STUDENT__ */ 