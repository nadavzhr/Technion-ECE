#include "stack.h"
#define EMPTY -1

typedef struct stack {
  size_t max_size;          /* maximum size of the stack.                  */
  long long head_index;     /* index to the head of the stack, may be -1   */
  elem_t *stack_arr;        /* data array that holds contents of the stack */
  clone_t clone_fn;         /* pointer to a user-made clone function       */
  destroy_t destroy_fn;     /* pointer to a user-made destroy function     */
  print_t print_fn;         /* pointer to a user-made print function       */
} Stack;


pstack_t stack_create(size_t max_num_of_elem, clone_t clone_fn,
                      destroy_t destroy_fn, print_t print_fn) {
  /* Build the stack */
  elem_t *content = malloc(max_num_of_elem*(sizeof(void*)));
  if (NULL == content) {
    fprintf(stderr, "Failed to allocate memory for the stack.\n");
    return NULL;
  }
  pstack_t new_stack = (pstack_t)malloc(sizeof(Stack));
  /* Set fields */
  new_stack->stack_arr = content;
  new_stack->max_size = max_num_of_elem;
  new_stack->head_index = EMPTY;
  new_stack->clone_fn = clone_fn;
  new_stack->destroy_fn = destroy_fn;
  new_stack->print_fn = print_fn;
  
  return (new_stack);
}


Result stack_destroy(pstack_t stack) {
  /* Check for errors */
  if (NULL == stack){
    fprintf(stderr, "No stack found to destroy.\n");
    return FAIL;
  }
  /* Pop the head until the stack is empty */
  while (!stack_is_empty(stack)) {
    stack_pop(stack);
  }
  /* Deallocate memory */
  free(stack->stack_arr); 
  free(stack);
  return SUCCESS;
}


Result stack_push(pstack_t stack, elem_t elem) {
  /* Check for errors */
  if (NULL == stack) {
    fprintf(stderr, "No stack found to push onto.\n");
    return FAIL;
  }
  if (NULL == elem) {
    fprintf(stderr, "No element found to push.\n");
    return FAIL;
  }
  if (stack_capacity(stack) <= 0) {
    fprintf(stderr, "This stack is full.\n");
    return FAIL;
  }
  /* Build the new elem and push a copy of it on top */
  elem_t new_elem = stack->clone_fn(elem);
  (stack->head_index)++;
  stack->stack_arr[stack->head_index] = new_elem;
  return SUCCESS;
}



void stack_pop(pstack_t stack) {
  /* Check for errors */
  if (NULL == stack) {
    fprintf(stderr, "No stack found.\n");
    return;
  }
  if (stack_is_empty(stack)) {
    fprintf(stderr, "Stack is empty.\n");

  }
  /* Pop */
  long long head = stack->head_index;
  elem_t e = stack->stack_arr[head];
  stack->destroy_fn(e);
  (stack->head_index)--;
}



elem_t stack_peek(pstack_t stack) {
  /* Check for errors */
  if (NULL == stack) {
    fprintf(stderr, "No stack found.\n");
    return NULL;
  }

  if (stack_is_empty(stack)) {
    fprintf(stderr, "Stack is empty.\n");
    return NULL;

  }
  /* Grab the uppermost element in the stack */
  long long head = stack->head_index;
  elem_t e = stack->stack_arr[head];
  return e;
}


size_t stack_size(pstack_t stack) {
  
  if (NULL == stack) {
    return 0;
  }
  return ((stack->head_index) + 1);
}



bool stack_is_empty(pstack_t stack) {
  
  if (NULL == stack) {
    return false;
  }
  if (stack->head_index <= EMPTY) {
    return true;
  }
  return false;
}



size_t stack_capacity(pstack_t stack) {

  if (NULL == stack) {
    return 0;
  }
  size_t current_size = stack_size(stack);
  return (stack->max_size - current_size);
}



void stack_print(pstack_t stack) {
  /* Check for errors */
  if (NULL == stack) {
    return;
  }
  if (stack_is_empty(stack)) {
    return;
  }
  long long head = stack->head_index;  /* temp running head index */
  while (head > EMPTY){
    stack->print_fn(stack->stack_arr[head]);
    head--;
  }
}
