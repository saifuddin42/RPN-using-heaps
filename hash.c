#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "node.h"
#include "hash.h"

/* Creates a new Symtab struct.
 * Return the pointer to the new symtab.
 * On any memory errors, return NULL
 */
Symtab *hash_initialize() {
  //Initialize Symtab from heap
  Symtab *symtab = malloc(sizeof(Symtab));
  if(symtab == NULL) {
    return NULL;
  }
  //Update Symtab values
  symtab->size = 0;
  symtab->capacity = HASH_TABLE_INITIAL;
  //Initialize Symbol **table from heap
  symtab->table = malloc(sizeof(Symbol *) * HASH_TABLE_INITIAL);

  if(symtab->table == NULL) {
    return NULL;
  }
  //Initialize all table values to NULL
  for(int i = 0; i < HASH_TABLE_INITIAL; i++) {
    symtab->table[i] = NULL;
  }

  return symtab;
}

/* Destroy your Symbol Table.
 * Return on any memory errors.
 */
void hash_destroy(Symtab *symtab) {
  
  if(symtab == NULL) {
    return;
  }

  Symbol *temp_symbol = NULL;
  //Iterate through each index of symtab->table using for loop
  for (int i = 0; i < symtab->capacity; i++) {

    Symbol *walker = symtab->table[i];
    //Iterate through each Symbol in the linked list and free them
    while (walker != NULL) {

      temp_symbol = walker;
      walker = walker->next;
      symbol_free(temp_symbol);
      temp_symbol = NULL;
    }
  }
  //free the table and symtab
  free(symtab->table);
  symtab->table = NULL;
  free(symtab);
  symtab = NULL;
}

/* Return the capacity of the table inside of symtab.
 * If symtab is NULL, return -1;
 */
int hash_get_capacity(Symtab *symtab) {
  
  if(symtab == NULL) {
    return -1;
  }

  return (symtab->capacity);
}

/* Return the number of used indexes in the table (size) inside of symtab.
 * If symtab is NULL, return -1;
 */
int hash_get_size(Symtab *symtab) {
  
  if(symtab == NULL) {
    return -1;
  }

  return (symtab->size);
}

/* Adds a new Symbol to the symtab via Hashing.
 * If symtab is NULL, there are any malloc errors, or if any rehash fails, return -1;
 * Otherwise, return 0;
 */
int hash_put(Symtab *symtab, char *var, int val) {
  
  if (symtab == NULL) {
      return -1;  
  }
  //Get the hash value of var in var_hash and calculate respective index
  long var_hash = hash_code(var);
  int index = var_hash % (symtab->capacity);

  //Create walker for traveral and set it to the index we get
  Symbol *walker = symtab->table[index];

  //Checks if the variable already exists at that index and if yes it just updates the value and return 0
  if(walker != NULL) {
    //Traverse through each Symbol in the linked list
    while(walker != NULL){

      if(!strcmp(walker->variable, var)){
        walker->val = val;
        return 0;
      }

      walker = walker->next;
    }
  }

  //In case the variable doesn't exist, create a new symbol and store it in temp_symbol
  Symbol *temp_symbol = symbol_create(var, val);

  //Check if this new insert made our table load increase more than 2.0 and rehash the table if yes
  double load = (symtab->size) / (symtab->capacity); 

  walker = symtab->table[index];

  //If index is null then directly insert at that index
  if(walker == NULL){
    symtab->table[index] = temp_symbol;
    (symtab->size)++;
  }
  else {
    //Else traverse through the linked list until you find the last item and and insert there
    while(walker->next != NULL) {
      walker = walker->next;
    }

    walker->next = temp_symbol;
    (symtab->size)++;
  }

  if(load >= 2.0) { 
    hash_rehash(symtab, (symtab->capacity) * 2);
  }

  return 0;
}

/* Gets the Symbol for a variable in the Hash Table.
 * On any NULL symtab or memory errors, return NULL
 */
Symbol *hash_get(Symtab *symtab, char *var) {

  if(symtab == NULL) {
    return NULL;
  }
  //Hash var and obtain the index
  long var_hash = hash_code(var);
  int index = var_hash % (symtab->capacity);

  Symbol *walker = symtab->table[index];
  //Start from that index and traverse the linked list in that index till you find the var or you reach the end
  while(walker != NULL) {
  
    if(!strcmp(walker->variable, var)) {
      Symbol *temp_symbol = symbol_copy(walker);
      walker = NULL;
      return temp_symbol;
    }
  
    walker = walker->next;
  }
  //If walker is Null that means nothing exists at that index so return Null
  return NULL;
}

/* Doubles the size of the Array in symtab and rehashes.
 * If there were any memory errors, set symtab->array to NULL
 * If symtab is NULL, return immediately.
 */
void hash_rehash(Symtab *symtab, int new_capacity) {

  if(symtab == NULL) {
    return;
  }
  //Initialize new_table from heap with new_capacity
  Symbol **new_table = malloc(sizeof(Symbol *) * new_capacity);
  
  if(new_table == NULL) {
    symtab->table = NULL;
    return;
  }
  //Set all values in new_table to NULL
  for (int i = 0; i < new_capacity; i++)
  {
    new_table[i] = NULL;
  }

  //Copy the old symtab->table and capacity to temporary variables for future use
  Symbol **old_table = symtab->table;
  int old_capacity = symtab->capacity;

  //Update the table in symtab to new_table, new_capacity and reset it's size (Basically to use hash_put again recursively and insert all the old symbols from old_table to this symtab with new_table)
  symtab->capacity = new_capacity;
  symtab->size = 0;
  symtab->table = new_table;

  //Initialize walker and prewalker for traversal
  Symbol *walker = NULL;
  Symbol *prewalker = NULL;

  //Traverse the old_table and use hash_put to insert these variables and vals to new indexes in the new_table and simultaneously free these symbols after each use
  for (int i = 0; i < old_capacity; i++) {

    walker = old_table[i];
    prewalker = old_table[i];

    while(walker != NULL) {

      int flag = hash_put(symtab, walker->variable, walker->val);
      
      if(flag != 0) {
        return;
      }

      walker = walker->next;
      symbol_free(prewalker);
      prewalker = walker; //will become null once walker is null
    }
  }

  //Free the old table
  free(old_table);
  old_table = NULL;
}

/* Function to print the symbol table 
 */
void hash_print_symtab(Symtab *symtab) {
  if(symtab == NULL) {
    return;
  }
  printf("|-----Symbol Table [%d size/%d cap]\n", symtab->size, symtab->capacity);

  int i = 0;
  Symbol *walker = NULL;

  /* Iterate every index, looking for symbols to print */
  for(i = 0; i < symtab->capacity; i++) {
    walker = symtab->table[i];
    /* For each found linked list, print every symbol therein */
    while(walker != NULL) {
      printf("| %10s: %d \n", walker->variable, walker->val);
      walker = walker->next;
    }
  }
  return;
}

/* This computes the hash function for a String
 */
long hash_code(char *var) {
  long code = 0;
  int i;
  int size = strlen(var);

  for(i = 0; i < size; i++) {
    code = (code + var[i]);
    if(size == 1 || i < (size - 1)) {
      code *= 128;
    }
  }

  return code;
}