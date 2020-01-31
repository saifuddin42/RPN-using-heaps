#include <stdio.h>
#include <stdlib.h>

#include "node.h"
#include "stack.h"

/* Create a new Stack_head struct on the Heap and return a pointer to it.
 * On any malloc errors, return NULL
 */
Stack_head *stack_initialize() {
  //Initialize head from heap
  Stack_head *head = malloc(sizeof(Stack_head));

  if(head == NULL) {
    return NULL;
  }
  //Set the default values for Stack head and return it
  else {
    head->count = 0;
    head->top = NULL;
    return head;
  }
}

/* Destroys the stack.
 * If head is NULL, return.
 */
void stack_destroy(Stack_head *head) {

  Node *temp = NULL;
  
  if(head == NULL) {
    return;
  }
  //Iterate through each node (temp) and its token (head->top->tok) and free them
  while (head->top != NULL) {
    temp = head->top;
    token_free(head->top->tok);
    head->top = head->top->next;
    node_free(temp);
  }
  //free the head struct itself
  free(head);
  head = NULL;
  return;
}

/* Push a new Token on to the Stack.
 * On any malloc errors, return -1.
 * If there are no errors, return 0.
 */
int stack_push(Stack_head *stack, Token *tok) {
  
  Node *temp_node = NULL;

  if((stack == NULL) || (tok == NULL)) {
    return -1;
  }
  //Create a new node (temp_node) with the token (tok)
  else {
    temp_node = node_create(tok);

    if(temp_node == NULL){
      return -1;
    }
    //Attach temp_node at the top of the stack and increase the stack size
    else{
      temp_node->next = stack->top;
      stack->top = temp_node;
      (stack->count)++;
      temp_node = NULL;
      return 0;
    }
  }
}

/* Pop a Token off of the Stack.
 * If the stack was empty, return NULL.
 */
Token *stack_pop(Stack_head *stack) {

  Token *temp_token = NULL;
  Node *temp_node = NULL;
  
  if ((stack == NULL) || (stack->top == NULL)) {
    return NULL;
  }
  //Store the actual top node and token in temp variables and after head->top is updated, free node and return token. Also reduce stack size.
  else {
    temp_token = stack->top->tok;
    temp_node = stack->top;
    stack->top = stack->top->next;
    (stack->count)--;
    node_free(temp_node);
    temp_node = NULL;
    return temp_token;
  }
}

/* Return the token in the stack node on the top of the stack
 * If the stack is NULL, return NULL.
 * If the stack is empty, return NULL.
 */
Token *stack_peek(Stack_head *stack) {

  if((stack == NULL) || (stack->top == NULL)) {
    return NULL;
  }
  else {
    return (stack->top->tok);
  }
}

/* Return the number of nodes in the stack.
 * If stack is NULL, return -1.
 * Return 1 if the stack is empty or 0 otherwise.
 */
int stack_is_empty(Stack_head *stack) {
  
  if(stack == NULL) {
    return -1;
  }

  if(stack->count == 0) {
    return 1;
  }
  else {
    return 0;
  }
}

/* It recurses the stack and prints out the tokens in reverse order
 * eg. top->2->4->1->8 will print at Stack: 8 1 4 2
 * eg. stack_push(5) will then print Stack: 8 1 4 2 5
 */

/* Recursive print. (Local function)
 * Base Case: node == NULL, return
 * Recursive Case: call print_node(node->next, print_data), then print node.
 */
static void print_node(Node *node) {
  if(node == NULL) {
    return;
  }
  token_print(node->tok);
  print_node(node->next);
  return;
}

/* Setup function for the recursive calls.  Starts printing with stack->top
 */
void stack_print(Stack_head *stack) {
  if(stack == NULL) {
    return;
  }
  printf("|-----Program Stack\n");
  printf("| ");
  print_node(stack->top);
  printf("\n");
  return;
}