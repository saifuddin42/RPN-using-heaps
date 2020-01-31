#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "stack.h"
#include "token.h"
#include "hash.h"

/* Local Function Declarations */
static int read_file(char *filename, char *line);
static int parse_token(Symtab *symtab, Stack_head *stack, Token *tok);
static void print_header(char *filename, int step);
static void print_step_header(int step);
static void print_step_footer(Symtab *symtab, Stack_head *stack);
static void print_step_output(int val);

/* Defines the largest line that can be read from a file */
#define MAX_LINE_LEN 255

/* Main function to run your program.
 * 1) Opens the file using the passed in filename.
 * -- If the file is not found (ie. fopen returns NULL), then exit(-1);
 * 2) Reads one line from the file.
 * -- The programs will all only be one line in size.
 * -- That line may be up to MAX_LINE_LEN long.
 * 3) Closes the file.
 * 4) Calls token_read_line(line, strlen(line))
 * -- This parses the line and prepares the tokens to be ready to get.
 * 5) While there are tokens remaining to parse: token_has_next() != 0
 * -- a) Get the next token: token_get_next()
 * 6) Parse the token (your function)
 * 7) Print out all of the relevant information
 */
int rpn(Stack_head *stack, Symtab *symtab, char *filename) {
  int step = 0; /* Used to track the program steps */
  int ret = 0;
  char line[MAX_LINE_LEN];
  Token *tok = NULL;

  /* Complete the read_file function that is defined later in this file. */
  ret = read_file(filename, line);
  if(ret != 0) {
    printf("Error: Cannot Read File %s.  Exiting\n", filename);
    exit(-1);
  }

  /* Pass the line into the tokenizer to initialize that system */
  token_read_line(line, strlen(line));

  /* Prints out the nice program output header */
  print_header(filename, step);

  /* Iterate through all tokens */
  while(token_has_next()) {
    /* Begin the next step of execution and print out the step header */
    step++; /* Begin the next step of execution */
    print_step_header(step);

    /* Get the next token */
    tok = token_get_next();
    /* Complete the implementation of this function later in this file. */
    ret = parse_token(symtab, stack, tok);
    if(ret != 0) {
      printf("Critical Error in Parsing.  Exiting Program!\n");
      exit(-1);
    }

    /* Prints out the end of step information */
    print_step_footer(symtab, stack);
  }

  return 0;
}

/* Local function to open a file or exit.
 * Open filename, read its contents (up to MAX_LINE_LEN) into line, then
 *   close the file and return 0.
 * On any file error, return -1.
 */
static int read_file(char *filename, char *line) {

  if(filename == NULL) {
    return -1;
  }
  //Open file in read mode
  FILE *fp = fopen(filename, "r");

  if(fp == NULL) {
    return -1;
  }

  while(feof(fp) == 0) { // Run until EOF is reached
    fgets(line, MAX_LINE_LEN, fp); // Get one line from file
  }

  fclose(fp);
  return 0;
}

/* Parses the Token to implement the rpn calculator features
 * You may implement this how you like, but many small functions would be good!
 * If the token you are passed in is NULL, return -1.
 * If there are any memory errors, return -1.
 */
static int parse_token(Symtab *symtab, Stack_head *stack, Token *tok) {

  int flag = -1;
  Token *tok_temp = NULL;
  Token *tok_temp1 = NULL;
  Token *tok_temp2 = NULL;
  Token *tok_temp3 = NULL;
  int temp1 = -1, temp2 = -1, temp3 = -1;

  if (symtab == NULL || stack == NULL || tok == NULL) {
    return -1;
  }

  //Switch case for the type of token
  switch (tok->type) {

  case TYPE_ASSIGNMENT:
  //Pop two tokens off the stack. 
    tok_temp1 = stack_pop(stack);
    tok_temp2 = stack_pop(stack);

    if (tok_temp1 == NULL || tok_temp2 == NULL) {
      return -1;
    }

    temp1 = tok_temp1->value;

    //If tok_temp1 is a variable, then search for its value in hash table and assign it to variable of tok_temp2
    if(tok_temp1->type == TYPE_VARIABLE) {
      Symbol *temp_symbol = hash_get(symtab, tok_temp1->variable);
      temp1 = temp_symbol->val;      
      symbol_free(temp_symbol);
      temp_symbol = NULL;
    }

    //Assign the value of tok_temp1 to variable of tok_temp2.
    flag = hash_put(symtab, tok_temp2->variable, temp1);

    if (flag != 0) {
      return -1;
    }

    token_free(tok_temp1);
    tok_temp1 = NULL;
    token_free(tok_temp2);
    tok_temp2 = NULL;
    token_free(tok);
    tok = NULL;
    break;

  case TYPE_OPERATOR:
    tok_temp1 = stack_pop(stack);
    tok_temp2 = stack_pop(stack);

    if (tok_temp1 == NULL || tok_temp2 == NULL) {
      return -1;
    }

    //Depending on the type of token, get the values of from them and assign it to temporary variables
    if(tok_temp1->type == TYPE_VALUE) {
      temp1 = tok_temp1->value;
    }
    if(tok_temp1->type == TYPE_VARIABLE) {
      Symbol *temp_symbol1 = hash_get(symtab, tok_temp1->variable);
      temp1 = temp_symbol1->val;
      symbol_free(temp_symbol1);
      temp_symbol1 = NULL;
    }
    if(tok_temp2->type == TYPE_VALUE) {
      temp2 = tok_temp2->value;
    }
    if(tok_temp2->type == TYPE_VARIABLE) {
      Symbol *temp_symbol2 = hash_get(symtab, tok_temp2->variable);
      temp2 = temp_symbol2->val;
      symbol_free(temp_symbol2);
      temp_symbol2 = NULL;
    }

    //Switch case for the type of operation
    switch (tok->oper) {

    case OPERATOR_PLUS:
      temp3 = temp2 + temp1;
      break;

    case OPERATOR_MINUS:
      temp3 = temp2 - temp1;
      break;

    case OPERATOR_MULT:
      temp3 = temp2 * temp1;
      break;

    case OPERATOR_DIV:
      temp3 = temp2 / temp1;
      break;

    default:
      return -1;
    }

    //Create a token with the answer value
    tok_temp3 = token_create_value(temp3);

    if (tok_temp3 == NULL) {
      return -1;
    }
    //Push this new token on the stack
    flag = stack_push(stack, tok_temp3);

    if (flag == -1) {
      return -1;
    }

    token_free(tok_temp1);
    tok_temp1 = NULL;
    token_free(tok_temp2);
    tok_temp2 = NULL;
    token_free(tok);
    tok = NULL;
    break;

  case TYPE_VARIABLE:
    //Push this variable on the stack
    flag = stack_push(stack, tok);
    if (flag != 0) {
      return -1;
    }
    break;

  case TYPE_VALUE:
    //Push this value on the stack
    flag = stack_push(stack, tok);
    if (flag != 0) {
      return -1;
    }
    break;

  case TYPE_PRINT:
    tok_temp = stack_pop(stack);

    if (tok_temp == NULL) {
      return -1;
    }
    //If the popped token is just a value, print it as it is
    if (tok_temp->type == TYPE_VALUE) {
      print_step_output(tok_temp->value);
    }
    //If the popped token is a variable, get its value from hash table and print it
    if (tok_temp->type == TYPE_VARIABLE) {
      Symbol * temp_sym = hash_get(symtab, tok_temp->variable);
      
      if (temp_sym == NULL) {
        return -1;
      }

      print_step_output(temp_sym->val);
      symbol_free(temp_sym);
      temp_sym = NULL;
    }

    token_free(tok_temp);
    tok_temp = NULL;
    token_free(tok);
    break;

  default:
    return -1;
  }

  return 0;
}

/* Prints out the main output header
 */
static void print_header(char *filename, int step) {
  printf("######### Beginning Program (%s) ###########\n", filename);
  printf("\n.-------------------\n");
  printf("| Program Step = %2d\n", step);
  token_print_remaining();
  printf("o-------------------\n");
}

/* Prints out the information at the top of each step
 */
static void print_step_header(int step) {
  printf("\n.-------------------\n");
  printf("| Program Step = %2d\n", step++);
}

/* Prints out the output value (print token) nicely
 */
static void print_step_output(int val) {
  printf("|-----Program Output\n");
  printf("| %d\n", val);
}

/* Prints out the information at the bottom of each step
 */
static void print_step_footer(Symtab *symtab, Stack_head *stack) {
  hash_print_symtab(symtab);
  stack_print(stack);
  token_print_remaining();
  printf("o-------------------\n");
}