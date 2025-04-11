#ifndef __STACK__
#define __STACK__

#include <stddef.h>  /* size_t */
#include <stdbool.h> /* bool   */ 
#include "stdlib.h"
#include "stdio.h"

typedef enum { SUCCESS = 0, FAIL } Result; 

typedef struct stack *pstack_t;         /*  pointer to a stack             */
typedef void *elem_t;                   /*  pointer to a general element   */
typedef elem_t (*clone_t)(elem_t e);    /*  pointer to a clone function    */
typedef void (*destroy_t)(elem_t e);    /*  pointer to a destroy function  */
typedef void (*print_t)(elem_t e);      /*  pointer to a print function    */



/**
 * @fn pstack_t stack_create(size_t max_num_of_elem, 
 * clone_t clone_fn, destroy_t destroy_fn, print_t print_fn);
 * @brief create a new stack ADT, with the ability to perform all kind 
 * of actions on the given stack as listed in the header file.
 * @param max_num_of_elem max number of elements that can be held in 
 * the stack in any given time. 
 * @param clone_fn pointer to a user-made clone function.
 * @param destroy_fn pointer to a user-made destroy function.
 * @param print_fn pointer to a user-made print function.
 * @return pointer to a stack ADT.
 * NULL if it fails.
 */
pstack_t stack_create(size_t max_num_of_elem, clone_t clone_fn,
                        destroy_t destroy_fn, print_t print_fn);



/**
 * @fn void stack_destroy(pstack_t s);
 * @brief Destroys the user_stack. free all the memory of the elements in the
 * stack. user_stack must be a stack created by StackCreate otherwise
 * undefined behavior)
 * @param stack Pointer to the stack to destroy
 * @return None
 */
Result stack_destroy(pstack_t stack);




/**
 * @fn stack_push(pstack_t stack, elem_t new_elem);
 * @brief Adds a new stack element to the head of the stack.
 * @param stack Pointer to the stack want to add the element to.
 * @param new_elem Pointer that references the new element to 
 be added to the stack.
 * @return Fail or success.
*/
Result stack_push(pstack_t stack, elem_t new_elem);



/**
 * @fn stack_pop(pstack_t stack);
 * @brief Delete the last element that was added to the stack.
 * @param stack Pointer to the stack.
 * @return None
*/
void stack_pop(pstack_t stack);




/**
 * @fn stack_peek(pstack_t stack);
 * @brief Gives a pointer to the last element that was added.
 * @param stack Pointer to the stack.
 * @return Pointer to the last element, Null if fail.
*/
elem_t stack_peek(pstack_t stack);


/**
 * @fn stack_size(pstack_t stack);
 * @brief Gives back the current number of element in the stack.
 * @param stack Pointer to the stack.
 * @return size_t, number of current elements.
*/
size_t stack_size(pstack_t stack);


/**
 * @fn stack_is_empty(pstack_t stack);
 * @brief Checks if the stack is currently empty.
 * @param stack POinter to the stack.
 * @return True if stack is empty, else False.
*/
bool stack_is_empty(pstack_t stack);




/**
 * @fn stack_capacity(pstack_t stack);
 * @brief This function returns the number of available slots in the
  data structure where new elements can be inserted.
 * @param stack Pointer to the stack.
 * @return size_t, number of empty spaces.
*/
size_t stack_capacity(pstack_t stack);




/**
 * @fn void stack_print(pstack_t stack)
 * @brief Prints the elements of the stack in LIFO order.
 * @param stack Pointer to the stack.
 * @return None.
**/

void stack_print(pstack_t stack);


#endif /* __STACK__ */